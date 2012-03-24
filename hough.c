/*
 * Programmed by:   Robert Ainsley McLaughlin
 * E-mail:          ram@ee.uwa.edu.au
 * Date:            18 August, 1996
 * Organization:    The Center For Intelligent Information Processing Systems, Dept. Electrical & Electronic Engineering, The University of Western Australia,
 *                  Nedlands W.A. 6907, Australia
 *
 * The original code was refactored. It serves to detect ellipses over binary images and PBM format.
 * Modified by:     Edwin Delgado Huaynalaya
 * E-mail:          edychrist@gmail.com
 * Last modified:   February, 2012
 * Organization:    Institute of Mathematics and Statistics, University of S&atilde;o Paulo,
 *                  S&atilde;o Paulo, Brazil 
 *
 * Main function:   double* ellipse_hough_transform(char filename[],int window_size, double sample_proportion_for_local_max, int min_local_max_value,
 *                                                  double min_ellipse_proportion, int max_ellipse, int min_ellipse, double min_dist)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <values.h>     /* MAXINT */
#include <strings.h>
#include <stdarg.h>
#include <time.h>

extern double   sqrarg;
#define SQR(a)  ((sqrarg=(double )(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

extern double   absarg;
#define ABS(a)  ((absarg=(a)) > 0 ? absarg : -absarg)

double   sqrarg;    /* Used in the macro SQR() */
double   absarg;    /* Used in the macro ABS() */

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#define WHITE   63
#define BLACK   0

#define NO_ANGLE    -100
#define A_SMALL_NEIGHBOURHOOD   (2*8)
#define alloc_vector_MACRO(dim) ((double *)malloc(( (unsigned int)(dim+1) ) * sizeof(double)) )

typedef int BOOLEAN;

typedef struct hough_point_type {
    int         x, y;
    double      theta, slope;
    BOOLEAN     valid_angle;        /* is the value in theta, slope valid */
    struct hough_point_type *next;
} HOUGH_POINT;



typedef struct hough_point_list_type {
    HOUGH_POINT     *head;
    HOUGH_POINT     *tail;
    int             num_elements;   /* number of elements in linked list. */

} HOUGH_POINT_LIST;


typedef struct point_type {
    int dim;
    double  *x;         /* indexed [1..dim] */
    BOOLEAN flag;       /* generic flag - to be used for whatever
                         * seems important at the time.
                         */
    struct point_type   *next;
    struct point_type   *next_list;
} POINT;



typedef struct point_list_type {
    POINT   *head;
    POINT   *tail;
    int     num_elements;   /* number of elements in linked list. */
    struct  point_list_type *next_list;

} POINT_LIST;



typedef struct ellipse_type {
    /* An ellipse centred on (x,y)
     * with major axis of length a at angle theta
     * and minor axis of length b.
     *
     * If the co-ordinate system were translated
     * by (x, y) and then rotated by theta,
     * so as to centre the ellipse on the origin and
     * bring the major axis in line with the x-axis,
     * then the ellipse would be defined by the points:
     *     { (x,y) : (x/a)^2 + (y/b)^2 = 1}
     *
     * note: When I refer to the major axis, I mean
     *       the largest radius of the ellipse.
     *       The minor axis refers to the smallest
     *       radius of the ellipse.
     */

    double  x, y;
    double  a, b, theta;
    struct ellipse_type *next;

} ELLIPSE;




typedef struct image_type {
    unsigned int    x, y;   /* dimensions of image
                             * Allowable values for x are [0, x)
                             *                      y are [0, y)
                             */
    unsigned char   **data; /* The pixel values for the image.
                             * All values are in the range [0,63].
                             */
} IMAGE;


int
get_pixel_value(IMAGE *aImage, int x, int y)
{
    /* The lower 6 bits specify the value of the pixel.
     * The upper 2 bits are used for tagging a pixel.
     */
    return((int )(aImage->data[x][y] & (unsigned char )0x3F));

}   /* end of get_pixel_value() */


HOUGH_POINT *
alloc_hough_point(void)
{
    HOUGH_POINT *hp;

    hp = (HOUGH_POINT *)malloc(sizeof(HOUGH_POINT));
    if (hp == NULL)
        {
        fprintf(stderr, "malloc failed in alloc_hough_point() in sht_ellipse.c\n");
        exit(1);
        }

    hp->next = NULL;

    return(hp);

}       /* end of alloc_hough_point() */

HOUGH_POINT_LIST *
alloc_hough_point_list(void)
{
    HOUGH_POINT_LIST  *hough_point_list;


    hough_point_list = (HOUGH_POINT_LIST *)malloc( sizeof(HOUGH_POINT_LIST) );

    if (hough_point_list == NULL)
        {
        fprintf(stderr, "malloc failed in alloc_hough_point_list() in sht_ellipse.c\n");
        exit(1);
        }

    hough_point_list->head = NULL;
    hough_point_list->tail = NULL;
    hough_point_list->num_elements = 0;

    return(hough_point_list);

}   /* end of alloc_hough_point_list() */


POINT *
alloc_point(int dim)
{
    POINT   *p;


    p = (POINT *)malloc(sizeof(POINT));
    if (p == NULL)
        {
        fprintf(stderr, "malloc failed in alloc_point() in point.c\n");
        exit(1);
        }
    p->dim = dim;
    p->x = alloc_vector_MACRO(dim);
    if (p->x == NULL)
        {
        fprintf(stderr, "malloc failed in alloc_point() in point.c\n");
        exit(1);
        }
    p->flag = FALSE;
    p->next = NULL;
    p->next_list = NULL;

    return(p);

}   /* end of alloc_point() */



POINT_LIST *
put_in_linked_list_of_point(POINT_LIST *point_list, POINT *p)
{
    POINT_LIST  *alloc_point_list(void);


    if (point_list == NULL)
        point_list = alloc_point_list();

    if (point_list->head == NULL)
        {
        point_list->head = p;
        point_list->tail = p;
        point_list->num_elements = 1;
        }
    else
        {
        /* Add p to the list.
         */
        point_list->tail->next = p;

        /* Update the tail. */
        point_list->tail = p;

        /* Update the number of elements. */
        (point_list->num_elements)++;

        }

    p->next = NULL;

    return(point_list);


}   /* end of put_in_linked_list_of_point() */



POINT_LIST *
alloc_point_list(void)
{
    POINT_LIST  *point_list;


    point_list = (POINT_LIST *)malloc( sizeof(POINT_LIST) );

    if (point_list == NULL)
        {
        fprintf(stderr, "malloc failed in alloc_point_list() in point.c\n");
        exit(1);
        }

    point_list->head = NULL;
    point_list->tail = NULL;
    point_list->num_elements = 0;

    point_list->next_list = NULL;

    return(point_list);

}   /* end of alloc_point_list() */



ELLIPSE *
alloc_ellipse(void)
{
    ELLIPSE *e;


    e = (ELLIPSE *)malloc(sizeof(ELLIPSE));
    if (e == NULL)
        {
        fprintf(stderr, "malloc failed in alloc_ellipse() in sht_ellipse.c\n");
        exit(1);
        }

    e->next = NULL;

    return(e);

}       /* end of alloc_ellipse() */



ELLIPSE *
find_other_three_ellipse_parameters(POINT_LIST *centre_point_list, IMAGE *aImage, double min_ellipse_proportion, int max_ellipse, int min_ellipse, double min_dist)
{
    double      a_start, a_end, b_start, b_end, theta_start, theta_end;
    double      da, db, dtheta;
    int         a_index, b_index, theta_index;
    double      a_max, b_max, theta_max;
    int         i;
    ELLIPSE     *ellipse, *ellipse_list;
    int         ***histogram;
    POINT       *centre_point;
    double      proportion;
    int ***     alloc_3D_histogram(int x_max, int y_max, int z_max);
    void        other_three_parameters_SHT_hough_transform(int ***histogram, POINT *centre_point, double a_start, double a_end, double b_start, double b_end, double theta_start, double theta_end, IMAGE *aImage);
    void        find_max_of_3D_histogram(int ***histogram, int *x_val, int *y_val, int *z_val, int x_max, int y_max, int z_max);
    void        free_3D_histogram(int ***histogram, int x_max, int y_max);
    double      find_proportion_of_ellipse_that_exists(double x_e, double y_e, double a, double b, double theta, IMAGE *aImage, double min_dist);
    ELLIPSE     *alloc_ellipse(void);
    ELLIPSE     *put_in_linked_list_of_ellipse(ELLIPSE *head, ELLIPSE *e);
    double a_over_b;

    if ((centre_point_list == (POINT_LIST *)NULL) || (centre_point_list->head == (POINT *)NULL))
        return( (ELLIPSE *)NULL );

    ellipse_list = NULL;

    histogram = alloc_3D_histogram(20, 20, 20);

    for (centre_point=centre_point_list->head; centre_point != NULL;
                                                centre_point=centre_point->next)
        {
        a_start = b_start = 0.0;
        /* a_end, b_end define the size of the
         * largest ellipse that we can find.
         *
         * We have set this largest ellipse to have a
         * major radius 1/2 as large as the diagonal of the image.
         */
        a_end = b_end = max_ellipse / 2.0; //sqrt(SQR(aImage->x) + SQR(aImage->y)) / 2.0;
        theta_start = 0.0;  theta_end = M_PI;

        for (i=0; i < 5; i++)
            {
            other_three_parameters_SHT_hough_transform(histogram, centre_point, a_start, a_end, b_start, b_end, theta_start, theta_end, aImage);
            find_max_of_3D_histogram(histogram, &a_index, &b_index, &theta_index, 20, 20, 20);


            da = ((double )(a_end-a_start)) / 20;
            db = ((double )(b_end-b_start)) / 20;
            dtheta = ((double )(theta_end-theta_start)) / 20;

            a_max = a_start + da*(double )a_index;
            b_max = b_start + db*(double )b_index;
            theta_max = theta_start + dtheta*(double )theta_index;

            a_start = a_max - da;
            if (a_start < 0)
                a_start = 0;
            a_end = a_max + da;
            b_start = b_max - db;
            if (b_start < 0)
                b_start = 0;
            b_end = b_max + db;
            theta_start = theta_max - dtheta;
            theta_end = theta_max + dtheta;
#ifdef COMPUTATION_TIME
            check_parity();
#endif
            }

        /* Having found the parameters for an ellipse,
         * we need to check how much of the ellipse exists.
         * If only a small bit of it exists, then the ellipse
         * probably does not exist.
         * We are not talking about occluded ellipses here.
         * We are trying to detect when the hough transform
         * has taken a few black pixels from around the image
         * and interpretted them as bits of an ellipse.
         */
        proportion = find_proportion_of_ellipse_that_exists(centre_point->x[1], centre_point->x[2], a_max, b_max, theta_max, aImage, min_dist);
        if(a_max != 0 && b_max != 0){
          if(a_max > b_max){
	    a_over_b = a_max / b_max;	    	
	  } else {
            a_over_b = b_max / a_max;
          }
        } else {
          a_over_b = 0;
        }
        if ( (proportion > min_ellipse_proportion) && (a_max >= min_ellipse/2.0 && b_max >= min_ellipse/2.0 ) && a_over_b >= 1.50 ) 
            {

            ellipse = alloc_ellipse();
            ellipse->x = centre_point->x[1];
            ellipse->y = centre_point->x[2];
            ellipse->a = a_max;
            ellipse->b = b_max;
            ellipse->theta = theta_max;
            ellipse_list = put_in_linked_list_of_ellipse(ellipse_list, ellipse);
            }

#ifdef COMPUTATION_TIME
        check_parity();
#endif
        }

    free_3D_histogram(histogram, 20, 20);

    return(ellipse_list);

}   /* end of find_other_three_ellipse_parameters */

void
free_3D_histogram(int ***histogram, int x_max, int y_max)
{
    int x, y;

    for (x=0; x < x_max; x++)
        {
        for (y=0; y < y_max; y++)
            free(histogram[x][y]);
        free(histogram[x]);
        }

    free(histogram);

}   /* end of free_3D_histogram() */


int ***
alloc_3D_histogram(int x_max, int y_max, int z_max)
{
    int x, y;
    int ***histogram;

    /* Allocate space for a 3D histogram
     * histogram is an array of pointers,
     * each of which points to an array of pointers,
     * each of which points to an array of data.
     * So, histogram is a (int ***)
     *     histogram[] is a (int **)
     *     histogram[][] is a (int *)
     *     histogram[][][] is a (int )
     */

    histogram = (int ***)malloc((int )x_max * sizeof(int **));
    if (histogram == NULL)
        {
        fprintf(stderr, "first malloc failed in alloc_3D_histogram() in sht_ellipse.c\n");
        exit(1);
        }

    for (x=0; x < x_max; x++)
        {
        histogram[x] = (int **)malloc((int )y_max * sizeof(int *));
        if (histogram[x] == NULL)
            {
            fprintf(stderr, "second malloc failed in alloc_3D_histogram() in sht_ellipse.c\n");
            exit(1);
            }
        for (y=0; y < y_max; y++)
            {
            histogram[x][y] = (int *)malloc((int )z_max * sizeof(int ));
            if (histogram[x][y] == NULL)
                {
                fprintf(stderr, "third malloc failed in alloc_3D_histogram() in sht_ellipse.c\n");
                exit(1);
                }
            }
        }


    return(histogram);

}   /* end of alloc_3D_histogram() */



void
other_three_parameters_SHT_hough_transform(int ***histogram, POINT *centre_point, double a_start, double a_end, double b_start, double b_end, double theta_start, double theta_end, IMAGE *aImage)
{
    int     x, y;
    double  x1, y1, x2, y2;
    double  a, b, theta, da, db, dtheta;
    int     a_index, b_index, theta_index;
    int     get_pixel_value(IMAGE *aImage, int x, int y);

    /* Initialise all histogram values
     * to zero.
     */
    for (a_index=0; a_index < 20; a_index++)
        for (b_index=0; b_index < 20; b_index++)
            for (theta_index=0; theta_index < 20; theta_index++)
                histogram[a_index][b_index][theta_index] = 0;


    da = ((double )(a_end-a_start)) / 20;
    db = ((double )(b_end-b_start)) / 20;
    dtheta = ((double )(theta_end-theta_start)) / 20;

    for (x=0; x < aImage->x; x++)
        for (y=0; y < aImage->y; y++)
            if (get_pixel_value(aImage,x,y) == BLACK)
                {
                /* Translate (x,y) relative to centre of ellipse.
                 */
                x1= ((double )x)-centre_point->x[1];
                y1 = ((double )y)-centre_point->x[2];

                for (a_index=0, a=a_start; a_index < 20; a_index++, a+=da)
                    for (theta_index=0, theta=theta_start; theta_index < 20; theta_index++, theta+=dtheta)
                        {
                        /* Given a and theta, calculate b
                         */

                        /* Rotate (x1,y1) relative to theta.
                         */
                        x2 = x1*cos(theta) + y1*sin(theta);
                        y2 = -x1*sin(theta) + y1*cos(theta);

                        if (SQR(x2/a) < 1.0)    /* avoid divide by zero */
                            {
                            b = sqrt( SQR(y2) / (1.0 - SQR(x2/a)) );
                            b_index = (int )((b-b_start) / db);
                            if ((b_index >= 0) && (b_index < 20))
                                histogram[a_index][b_index][theta_index]++;
                            }
                        }
                }


}   /* end of other_three_parameters_SHT_hough_transform() */


void
find_max_of_3D_histogram(int ***histogram, int *x_val, int *y_val, int *z_val, int x_max, int y_max, int z_max)
{
    int     x, y, z;
    int     max_val;


    *x_val = 0;  *y_val = 0;  *z_val = 0;
    max_val = 0;

    for (x=0; x < x_max; x++)
        for (y=0; y < y_max; y++)
            for (z=0; z < z_max; z++)
                if (histogram[x][y][z] > max_val)
                    {
                    max_val = histogram[x][y][z];
                    *x_val = x;
                    *y_val = y;
                    *z_val = z;
                    }


}   /* end of find_max_of_3D_histogram() */

double
find_proportion_of_ellipse_that_exists(double x_e, double y_e, double a, double b, double theta, IMAGE *aImage, double min_dist)
{
    int     x, y;
    double  x1, y1, x2, y2;
    double  count = 0.0;
    double  perimeter;
    int     get_pixel_value(IMAGE *aImage, int x, int y);

    for (x=0; x < aImage->x; x++)
        for (y=0; y < aImage->y; y++)
            if (get_pixel_value(aImage,x,y) == BLACK)
                {
                x1 = (double )x - x_e;  /* translate pixel relative */
                y1 = (double )y - y_e;  /* to ellipse centre.       */

                /* Rotate (x1,y1) relative to theta.
                 */
                x2 = x1*cos(theta) + y1*sin(theta);
                y2 = -x1*sin(theta) + y1*cos(theta);

                /* Scale it relative to a, b
                 */
                x2 /= a;
                y2 /= b;

                /* If (x,y) lies on or near the ellipse,
                 * then (x2,y2) will lie on or near
                 * the unit circle.
                 */
                if (ABS(1.0 - (SQR(x2) + SQR(y2))) < min_dist) 
                    {
                    count++;
                    }
                }


    /* Note: we multiply the ellipse perimeter by the current
     * line thickness.
     */

    perimeter = M_PI * ( 3*(a+b) - sqrt((a+3*b)*(3*a+b)) ) * 2;

    return(count/perimeter);

}   /* end of find_proportion_of ellipse_that_exists() */


ELLIPSE *
put_in_linked_list_of_ellipse(ELLIPSE *head, ELLIPSE *e)
{
    ELLIPSE *ptr;

    if (head == NULL)
        head = e;
    else
        {
        for (ptr=head; ptr->next != NULL; ptr=ptr->next)
            ;
        ptr->next = e;
        }

    e->next = NULL;


    return(head);

}   /* end of put_in_linked_list_of_ellipse() */



void
free_linked_list_of_point(POINT_LIST *point_list)
{
    POINT   *p1, *p2;
    void    free_point(POINT *p);

    if  (point_list != NULL)
        {
        p1 = point_list->head;
        while (p1 != NULL)
            {
            p2 = p1;
            p1 = p1->next;
            free_point(p2);
            }

        free(point_list);
        }


}   /* end of free_linked_list_of_point() */

void
free_point(POINT *p)
{
    if (p != NULL)
        {
        if (p->x != NULL)
            free(p->x);
        free(p);
        }

}   /* end of free_point() */



void
average_histogram(int **histogram, int x_max, int y_max)
{
    int     **temp_histogram;
    int     x, y, i, j;
    int     sum;
    int     **alloc_histogram(int x_max, int y_max);
    
    temp_histogram = alloc_histogram(x_max, y_max);

    for (x=0; x < x_max; x++)
        for (y=0; y < y_max; y++)
            temp_histogram[x][y] = histogram[x][y];

    for (x=1; x < x_max-1; x++)
        for (y=1; y < y_max-1; y++)
            {
            sum = 0;
            for (i=x-1; i <= x+1; i++)
                for (j=y-1; j <= y+1; j++)
                    sum += temp_histogram[i][j];
            histogram[x][y] = sum / 9;
            }

}   /* end of average_histogram() */


void
free_hough_point(HOUGH_POINT *hp)
{
    /* Admittedly, this function is a bit of a waste of time,
     * but I put it in since I have a free function for
     * my other data structures, e.g.
     * free_gaus(), free_histogram(), free_point(), free_basis(), free_image()
     */

    free(hp);

}   /* end of free_hough_point() */



void
free_linked_list_of_hough_point(HOUGH_POINT_LIST *hough_point_list)
{
    HOUGH_POINT   *hp1, *hp2;
    void    free_hough_point(HOUGH_POINT *p);

    if  (hough_point_list != NULL)
        {
        hp1 = hough_point_list->head;
        while (hp1 != NULL)
            {
            hp2 = hp1;
            hp1 = hp1->next;
            free_hough_point(hp2);
            }

        free(hough_point_list);
        }


}   /* end of free_linked_list_of_hough_point() */

double
find_tangent_of_edge(IMAGE *aImage, int x_p, int y_p)
{
    int     num_pixels;
    int     x, y, x_min, x_max, y_min, y_max;
    int     sum1, sum2;
    double  slope, theta;
    int     get_pixel_value(IMAGE *aImage, int x, int y);

    x_min = x_p - A_SMALL_NEIGHBOURHOOD;
    if (x_min < 0)
        x_min = 0;
    x_max = x_p + A_SMALL_NEIGHBOURHOOD;
    if (x_max >= aImage->x)
        x_max = aImage->x-1;

    y_min = y_p - A_SMALL_NEIGHBOURHOOD;
    if (y_min < 0)
        y_min = 0;
    y_max = y_p + A_SMALL_NEIGHBOURHOOD;
    if (y_max >= aImage->y)
        y_max = aImage->y-1;


    num_pixels = 0;
    sum1 = sum2 = 0;
    for (x=x_min; x <= x_max; x++)
        for (y=y_min; y <= y_max; y++)
            if (get_pixel_value(aImage, x, y) == BLACK)
                {
                num_pixels++;
                sum1 += (x-x_p)*(y-y_p);
                sum2 += (x-x_p)*(x-x_p);
                }

    /* The slope of the best line through the
     * black pixels in this neighbourhood
     * is sum1 / sum2.
     * We will convert this to an angle.
     */
    if (num_pixels == 1)
        {
        /* If there was only one black pixel
         * i.e. at co-ord (x_p, y_p)
         * In that case, we can't tell what the
         * angle is at that point, so we will
         * return the value NO_ANGLE
         */
        theta = NO_ANGLE;
        }
    else if (sum2 == 0)
        {
        /* If all points were on a vertical line.
         * In this case, set the angle to M_PI_2 radians.
         */
        theta = M_PI_2;
        }
    else
        {
        slope = ((double )sum1) / (double )sum2;
        theta = atan(slope);
        }


    return(theta);

}   /* end of find_tangent_of_edge() */


HOUGH_POINT_LIST *
put_in_linked_list_of_hough_point(HOUGH_POINT_LIST *hough_point_list,
                                                            HOUGH_POINT *hp)
{
    HOUGH_POINT_LIST  *alloc_hough_point_list(void);


    if (hough_point_list == NULL)
        hough_point_list = alloc_hough_point_list();

    if (hough_point_list->head == NULL)
        {
        hough_point_list->head = hp;
        hough_point_list->tail = hp;
        hough_point_list->num_elements = 1;
        }
    else
        {
        /* Add hp to the list.
         */
        hough_point_list->tail->next = hp;

        /* Update the tail. */
        hough_point_list->tail = hp;

        /* Update the number of elements. */
        (hough_point_list->num_elements)++;

        }

    hp->next = NULL;

    return(hough_point_list);


}   /* end of put_in_linked_list_of_hough_point() */



IMAGE *
load_pbm_image(char filename[])
{
    IMAGE   *aImage;
    FILE    *infile;
    char    magic_num[10];
    int     width, height, x, y, data, i;
    unsigned char   data_raw, value;
    int     check_for_comment(FILE *infile);
    IMAGE   *alloc_IMAGE(int x, int y);
    void    set_IMAGE_pixel_value(IMAGE *aImage, int x, int y, int val);


    infile = fopen(filename, "r");
    if (infile == NULL)
        {
        fprintf(stderr, "Error opening file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }


    if (check_for_comment(infile) == EOF)
        {
        fprintf(stderr, "Error reading file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }

    if (fscanf(infile, "%[P14]", magic_num) != 1)
        {
        fprintf(stderr, "Error reading magic number of file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }

    if ((strlen(magic_num) != 2) ||
                            ((magic_num[1] != '1') && (magic_num[1] != '4')))
        {
        fprintf(stderr, "Error reading magic number of file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }

    if (check_for_comment(infile) == EOF)
        {
        fprintf(stderr, "Error reading file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }

    if (fscanf(infile, "%d", &width) != 1)
        {
        fprintf(stderr, "Error reading width in file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }

    if (check_for_comment(infile) == EOF)
        {
        fprintf(stderr, "Error reading file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }

    if (fscanf(infile, "%d", &height) != 1)
        {
        fprintf(stderr, "Error reading height in file %s in load_pbm_image() in image.c\n", filename);
        exit(1);
        }

    aImage = alloc_IMAGE(width, height);

    /* Read in the data
     */
    if (magic_num[1] == '1')
        {
        if (check_for_comment(infile) == EOF)
            return((IMAGE *)NULL);
        for (y=0; y < height; y++)
            {
            /* Read in a line of data.
             */
            for (x=0; x < width; x++)
                {
                if (fscanf(infile, "%d", &data) != 1)
                    {
                    fprintf(stderr, "Error reading data from file %s in load_pbm_image() in image.c\n", filename);
                    exit(1);
                    }
                if (data == 1)
                    set_IMAGE_pixel_value(aImage, x, y, BLACK);
                else if (data == 0)
                    set_IMAGE_pixel_value(aImage, x, y, WHITE);
                check_for_comment(infile);
                }
            }
        }
    else if (magic_num[1] == '4')
        {
        /* Discard white space after height.
         */
        fscanf(infile, "%*c");

        for (y=0; y < height; y++)
            {
            /* Read in a line of data.
             */
            for (x=0; x < width; x+=8)
                {
                if (fscanf(infile, "%c", &data_raw) != 1)
                    {
                    fprintf(stderr, "Error reading data from file %s in load_pbm_image() in image.c\n", filename);
                    exit(1);
                    }

                /* Extract each bit as the value for a pixel.
                 */
                for (i=0; i < 8; i++)
                    {
                    value = data_raw >> (7-i);
                    value = value & 0x01;       /* Mask 00000001 */
                    if (value == (unsigned char )1)
                        set_IMAGE_pixel_value(aImage, x+i, y, BLACK);
                    else if (value == (unsigned char )0)
                        set_IMAGE_pixel_value(aImage, x+i, y, WHITE);
                    }
                }
            }
        }

    return(aImage);


}   /* end of load_pbm_image() */

int
check_for_comment(FILE *infile)
{
    unsigned char   c;


    /* Remove trailing white characters.
     */
    do
        c = getc(infile);
    while ((c == '\n') || (c == ' '));


    /* If line begins with #
     * discard it - it is a comment.
     */
    while (c == '#')
        {
        /* Read in the rest of the line as comment
         */
        do
            {
            c = (unsigned char )getc(infile);
            }
        while ((c != (unsigned char )'\n') && (c != (unsigned char )EOF));
        c = getc(infile);   /* to check if next line begins with # */
        }

    /* When testing to see if the next line began with a #,
     * we read one too many characters.
     */
    if (c != (unsigned char )EOF)
        {
        if (ungetc(c, infile) == EOF)
            {
            fprintf(stderr, "Error reading pbm file in check_for_comment() in image.c\n");
            exit(1);
            }
        }


    if (c == (unsigned char )EOF)
        return(EOF);
    else
        return(!EOF);

}   /* end of check_for_comment() */

IMAGE *
alloc_IMAGE(int x, int y)
{
    IMAGE   *aImage;
    int     i;


    aImage = (IMAGE *)malloc(sizeof(IMAGE ));
    if (aImage == NULL)
        {
        fprintf(stderr, "malloc #1 failed in alloc_IMAGE() in image.c\n");
        exit(1);
        }

    aImage->x = x;
    aImage->y = y;

    /* aImage->data is an array of pointers.
     * aImage->data[i] points to a block of memory which contains
     * the values for the i'th column in the IMAGE.
     * The pixel (x,y) is accessed with aImage->data[x][y]
     */
    aImage->data = (unsigned char **)malloc(x * sizeof(unsigned char *) );
    if (aImage->data == NULL)
        {
        fprintf(stderr, "malloc #2 failed in alloc_IMAGE() in image.c\n");
        exit(1);
        }
    for (i=0; i < x; i++)
        {
        aImage->data[i] = (unsigned char *)malloc(y * sizeof(unsigned char) );
        if (aImage->data[i] == NULL)
            {
            fprintf(stderr, "malloc #3 failed in alloc_IMAGE() in image.c\n");
            exit(1);
            }
        }


    return(aImage);

}   /* end of alloc_IMAGE() */

void
set_IMAGE_pixel_value(IMAGE *aImage, int x, int y, int val)
{

    if ((x >= 0) && (x < aImage->x) && (y >= 0) && (y < aImage->y))
        {
        /* Clear the lower 6 bits. */
        aImage->data[x][y] &= (unsigned char )0xC0;

        /* Set the value. */
        aImage->data[x][y] |= (unsigned char )val;

        }

}   /* end of set_IMAGE_pixel_value() */



double* find_ellipses_with_SHT(IMAGE *aImage,int window_size, double sample_proportion_for_local_max, int min_local_max_value, double min_ellipse_proportion, int max_ellipse, int min_ellipse, double min_dist)
{
    int     **histogram;
    POINT_LIST   *centre_point_list;
    ELLIPSE *ellipse, *ellipse_list;

    int     num_ellipses;

    int     **alloc_histogram(int x_max, int y_max);
    void    ellipse_centre_SHT_hough_transform(IMAGE *aImage, int **histogram);
    void    average_histogram(int **histogram, int x_max, int y_max);
    POINT_LIST      *find_local_max_of_histogram(int win_size, int min_local_max_value, double sample_proportion_for_local_max, int **histogram, int x_max, int y_max);
    ELLIPSE *find_other_three_ellipse_parameters(POINT_LIST *centre_point_list, IMAGE *aImage, double min_ellipse_proportion, int max_ellipse, int min_ellipse, double min_dist);
    void    free_linked_list_of_ellipse(ELLIPSE *ellipse_head);
    void    free_linked_list_of_point(POINT_LIST *point_list);

    int i,j;

    double *ellipse_matrix = (double *)malloc(20 * 5 * sizeof(double));
    for(i=0;i<20;i++){
      for(j=0;j<5;j++){
         ellipse_matrix[i*5+j] = 0.0;
      }
    }

    num_ellipses = 0;
    histogram = alloc_histogram(aImage->x, aImage->y);
    ellipse_centre_SHT_hough_transform(aImage, histogram);

    average_histogram(histogram, aImage->x, aImage->y);

    centre_point_list = find_local_max_of_histogram(window_size,min_local_max_value,sample_proportion_for_local_max,histogram, aImage->x, aImage->y);

    ellipse_list = find_other_three_ellipse_parameters(centre_point_list, aImage, min_ellipse_proportion, max_ellipse, min_ellipse, min_dist);
    
    free_linked_list_of_point(centre_point_list);

    int k = 5;
    for (ellipse=ellipse_list; ellipse != NULL; ellipse=ellipse->next)
        {
        ellipse_matrix[num_ellipses*k+0] = ellipse->x;
        ellipse_matrix[num_ellipses*k+1] = ellipse->y;
        ellipse_matrix[num_ellipses*k+2] = ellipse->a;
        ellipse_matrix[num_ellipses*k+3] = ellipse->b;
        ellipse_matrix[num_ellipses*k+4] = ellipse->theta;
        num_ellipses++;

        }

    return ellipse_matrix;
}   /* end of find_ellipses_with_SHT() */


POINT_LIST *
find_local_max_of_histogram(int win_size, int min_local_max_value, double sample_proportion_for_local_max, int **histogram, int x_max, int y_max)
{
    int         x, y, i, j;
    int         x_c, y_c, val_c, num_tests, count;
    double      average_value;
    BOOLEAN     max_value_flag;
    POINT       *point;
    POINT_LIST  *centre_point_list;
    POINT       *alloc_point(int dim);
    POINT_LIST  *put_in_linked_list_of_point(POINT_LIST *point_list, POINT *p);



    /* Linked list recording local maxima of histogram */
    centre_point_list = (POINT_LIST *)NULL;

    for (x=0; x < x_max-win_size; x++)
            for (y=0; y < y_max-win_size; y++)
                {
                /* Check that we are not near another local maximum.
                 */
                if (histogram[x][y] == -1)
                    continue;
                if (histogram[x][y+win_size-1] == -1)
                    continue;


                /* Co-ords of window centre */
                x_c = x + win_size/2;
                y_c = y + win_size/2;
                val_c = histogram[x_c][y_c];

                if (val_c < min_local_max_value)
                    continue;

                /* Sub-sample histogram values in window, to
                 * test if centre value is big.
                 * Also, estimate the average histogram value.
                 * Test if centre value is twice as big as average
                 * value.
                 */
                max_value_flag = TRUE;
                average_value = 0;
                num_tests = (int )(SQR(win_size)*
                                            sample_proportion_for_local_max);
                for (count=0; count < num_tests; count++)
                    {
                    i = (int )(drand48() * (double )win_size);
                    j = (int )(drand48() * (double )win_size);
                    if (histogram[i+x][j+y] > val_c)
                        {
                        max_value_flag = FALSE;
                        break;              /* out of for-loop */
                        }
                    else
                        average_value += (double )histogram[i+x][j+y];
                    }

                /* Is it biggest value? */
                if (max_value_flag == FALSE)
                    continue;

                /* Is it larger than average? */
                average_value /= (double )num_tests;
                if ( ((double )val_c) < (2.0*average_value))
                    continue;

                /* If we are still here, than it is likely that
                 * we have found a local maxima.
                 * Exhaustively test if it is
                 * a local maxima.
                 */
                max_value_flag = TRUE;
                for (i=0; ((i < win_size) && (max_value_flag==TRUE)); i++)
                    for (j=0; j < win_size; j++)
                        if (histogram[i+x][j+y] > val_c)
                            {
                            max_value_flag = FALSE;
                            break;              /* out of j=0; j < ...' */
                            }
                if (max_value_flag == FALSE)
                    continue;

                /* If we made it this far, then we
                 * definately have a local maxima.
                 * Record it in a linked list of local maxima.
                 */
                point = alloc_point(2);
                point->x[1] = (double )x_c;
                point->x[2] = (double )y_c;
                centre_point_list = put_in_linked_list_of_point(centre_point_list, point);

                /* To stop us finding another local maximum near
                 * this one, we will mark all histogram entries
                 * in the window by setting them to -1.
                 */
                for (i=x; i < x+win_size; i++)
                    for (j=y; j < y+win_size; j++)
                        histogram[i][j] = -1;

                }


    return( centre_point_list );


}       /* end of find_local_max_of_histogram() */



void
ellipse_centre_SHT_hough_transform(IMAGE *aImage, int **histogram)
{
    HOUGH_POINT *hp, *hp1, *hp2;
    HOUGH_POINT_LIST    *hough_point_list;
    int         x, y, x_h, y_h;
    double      theta1, theta2;
    double      slope1, slope2;
    double      x1, y1, x2, y2, t1, t2, m1, m2;
    int         x_end, y_end;
    double      slope, b;
    int         get_pixel_value(IMAGE *aImage, int x, int y);
    HOUGH_POINT *alloc_hough_point(void);
    HOUGH_POINT_LIST    *put_in_linked_list_of_hough_point(
                                            HOUGH_POINT_LIST *hough_point_list,
                                            HOUGH_POINT *hp);
    void        free_linked_list_of_hough_point(
                                            HOUGH_POINT_LIST *hough_point_list);
    double  find_tangent_of_edge(IMAGE *aImage, int x, int y);




    hough_point_list = (HOUGH_POINT_LIST *)NULL;

    for (x=0; x < aImage->x; x++)
        for (y=0; y < aImage->y; y++)
            if (get_pixel_value(aImage,x,y) == BLACK)
                {
                /* Calculate angle of edge at this point.
                 * theta1 is in the range [-M_PI_2, M_PI_2].
                 */
                theta1 = find_tangent_of_edge(aImage, x, y);
                if (theta1 == NO_ANGLE)
                    {
                    /* Could not estimate angle of edge at the
                     * point (x,y)
                     */
                    continue;
                    }
                if (ABS(theta1) > 0.99*M_PI_2)
                    {
                    /* If tangent is almost vertical, then contiue.
                     * Soon, we will need to do equations with
                     * the slope (dy/dx) which will be near infinite
                     * for near vertical tangents.
                     */
                    continue;
                    }
                hp = alloc_hough_point();
                hp->x = x;
                hp->y = y;
                hp->theta = theta1;
                hp->slope = tan(theta1);

                hough_point_list = put_in_linked_list_of_hough_point(hough_point_list, hp);
                }

    /* No BLACK pixels in image. */
    if (hough_point_list == (HOUGH_POINT_LIST *)NULL)
        return;


    /* For every black pixel in the image
     */
    for (hp1=hough_point_list->head; hp1 != NULL; hp1=hp1->next)
        {
        x1 = (double )hp1->x;  y1 = (double )hp1->y;
        theta1 = hp1->theta;
        slope1 = hp1->slope;
        /* For every other black pixel in the image
         */
        for (hp2=hough_point_list->head; hp2 != NULL; hp2=hp2->next)
            {
            theta2 = hp2->theta;
            slope2 = hp2->slope;
            /* Make sure that theta2 < theta1
             */
            if (theta2 > theta1)
                theta2 -= M_PI;

            /* Don't bother finding meeting point
             * of tangents if the normals are almost
             * parallel or almost perpendicular.
             */
            if ( (((theta1-theta2) > (M_PI/10.0)) &&
                  ((theta1-theta2) < (4.0*M_PI/10.0)) ) ||
                        (((theta1-theta2) > (6.0*M_PI/10.0)) &&
                         ((theta1-theta2) < (9.0*M_PI/10.0))) )
                {
                /* (t1, t2) is the intersection point
                 * of the two tangents.
                 * (m1, m2) is the midpoint
                 * of (x,y) and (i,j)
                 */
                x2 = (double )hp2->x;  y2 = (double )hp2->y;

                t1 = (y1 - y2 - x1*slope1 + x2*slope2) / (slope2-slope1);
                t2 = (slope1*slope2*(x2-x1) - y2*slope1 + y1*slope2) / (slope2-slope1);
                m1 = (x1 + x2) / 2;
                m2 = (y1 + y2) / 2;
                /* We can assume that (m1, m2) is in the
                 * image as it is the midpoint of 2 points
                 * in the image: (x1,y1) and (x2,y2)
                 */

                /* Draw a line on the hough histogram.
                 * This line will pass through the
                 * ellipse centre.
                 * 
                 * If x co-ord varies more quickly
                 * than y co-ord (i.e. slope < 1),
                 * then index the line by the x co-ord.
                 */
                if (ABS(t2-m2) < ABS(t1-m1))
                    {
                    slope = (t2-m2) / (t1-m1);
                    b = (m2*t1 - m1*t2) / (t1-m1);

                    /* If (m1,m2) is to the left of (t1,t2)
                     */
                    if (m1 < t1)
                        {
                        if (slope == 0)
                            {
                            x_end = 0;
                            }
                        else if (slope > 0)
                            {
                            /* zero intercept
                             * i.e. y = 0
                             */
                            x_end = (int )(-b/slope);
                            }
                        else /* if slope < 0 */
                            {
                            /* y = edge of image
                             * i.e. y = aImage->y
                             */
                            x_end = (int )( (((double )aImage->y)-b) / slope );
                            }


                        /* Increase x_end by one.
                         * Otherwise, because of rounding errors,
                         * we may get a value of y_h=aImage->y
                         * which would give a go outside the bounds
                         * of the array histogram[][].
                         */
                        x_end++;


                        if (x_end < 0)
                            x_end = 0;


                        for (x_h=(int )m1; x_h > x_end; x_h--)
                            {
                            y_h = (int )(slope*(double )x_h + b);
                            histogram[x_h][y_h]++;
                            }
                        }
                    else    /* if m1 > t1 */
                        {
                    /* If (m1,m2) is to the right of (t1,t2)
                     */
                        if (slope == 0)
                            {
                            x_end = aImage->x;
                            }
                        else if (slope > 0)
                            {
                            /* y = edge of image
                             * i.e. y = aImage->y
                             */
                            x_end = (int )( (((double )aImage->y)-b) / slope );
                            }
                        else /* if slope < 0 */
                            {
                            /* zero intercept
                             * i.e. y = 0
                             */
                            x_end = (int )(-b/slope);
                            }


                        /* Decrease x_end by one.
                         * Otherwise, because of rounding errors,
                         * we may get a value of y_h=aImage->y
                         * which would give a go outside the bounds
                         * of the array histogram[][].
                         */
                        x_end--;


                        if (x_end > aImage->x)
                            x_end = aImage->x;

                        for (x_h=(int )m1; x_h < x_end; x_h++)
                            {
                            y_h = (int )(slope*(double )x_h + b);
                            histogram[x_h][y_h]++;
                            }
                        }
                    }
                else    /* ABS(t1-m1) <= ABS(t2-m2) */
                    {
                    /*
                     * y co-ord varies more quickly
                     * than x co-ord (i.e. slope > 1),
                     * so index the line by the y co-ord.
                     */
                    slope = (t1-m1) / (t2-m2);
                    b = (m1*t2 - m2*t1) / (t2-m2);

                    /* If (m1,m2) is below (t1,t2)
                     */
                    if (m2 < t2)
                        {
                        if (slope == 0)
                            {
                            y_end = 0;
                            }
                        else if (slope > 0)
                            {
                            /* zero intercept
                             * i.e. x = 0
                             */
                            y_end = (int )(-b/slope);
                            }
                        else /* if slope < 0 */
                            {
                            /* x = edge of image
                             * i.e. x = aImage->x
                             */
                            y_end = (int )( (((double )aImage->x)-b) / slope );
                            }


                        /* Increase y_end by one.
                         * Otherwise, because of rounding errors,
                         * we may get a value of x_h=aImage->x
                         * which would give a go outside the bounds
                         * of the array histogram[][].
                         */
                        y_end++;


                        if (y_end < 0)
                            y_end = 0;

                        for (y_h=(int )m2; y_h > y_end; y_h--)
                            {
                            x_h = (int )(slope*(double )y_h + b);
                            histogram[x_h][y_h]++;
                            }
                        }
                    else    /* if m2 > t2 */
                        {
                    /* If (m1,m2) is above (t1,t2)
                     */
                        if (slope == 0)
                            {
                            y_end = aImage->y;
                            }
                        else if (slope > 0)
                            {
                            /* x = edge of image
                             * i.e. x = aImage->x
                             */
                            y_end = (int )( (((double )aImage->x)-b) / slope );
                            }
                        else /* if slope < 0 */
                            {
                            /* zero intercept
                             * i.e. x = 0
                             */
                            y_end = (int )(-b/slope);
                            }


                        /* Decrease y_end by one.
                         * Otherwise, because of rounding errors,
                         * we may get a value of x_h=aImage->x
                         * which would give a go outside the bounds
                         * of the array histogram[][].
                         */
                        y_end--;


                        if (y_end > aImage->y)
                            y_end = aImage->y;

                        for (y_h=(int )m2; y_h < y_end; y_h++)
                            {
                            x_h = (int )(slope*(double )y_h + b);
                            histogram[x_h][y_h]++;
                            }
                        }
                    }

                }   /* end of if ((theta1-theta2 > M_PI/10.0)... */
            }   /* end of for loop based on hp2 */
#ifdef COMPUTATION_TIME
        //check_parity();
#endif
        }   /* end of for loop based on hp1 */


    free_linked_list_of_hough_point(hough_point_list);



}   /* end of ellipse_centre_SHT_hough_transform() */





int **
alloc_histogram(int x_max, int y_max)
{
    int x, y;
    int **histogram;


    /* Allocate space for the histogram.
     * histogram is an array of pointers to arrays.
     */
    histogram = (int **)malloc((int )x_max * sizeof(int *));
    if (histogram == NULL)
        {
        fprintf(stderr, "malloc failed in alloc_histogram() in sht_line.c\n");
        exit(1);
        }
    for (x=0; x < x_max; x++)
        {
        histogram[x] = (int *)malloc((int )y_max * sizeof(int) );
        if (histogram[x] == NULL)
            {
            fprintf(stderr, "malloc failed in alloc_histogram() in sht_line.c\n");
            exit(1);
            }
        }


    /* Initialise histogram
     */
    for (x=0; x < x_max; x++)
        for (y=0; y < y_max; y++)
            histogram[x][y] = 0;


    return(histogram);

}   /* end of alloc_histogram() */

double* ellipse_hough_transform(char filename[],int window_size, double sample_proportion_for_local_max, int min_local_max_value, double min_ellipse_proportion, int max_ellipse, int min_ellipse, double min_dist)
{
  IMAGE   *aImage;
  IMAGE   *load_pbm_image(char filename[]);
  double* find_ellipses_with_SHT(IMAGE *aImage,int window_size, double sample_proportion_for_local_max, int min_local_max_value, double min_ellipse_proportion, int max_ellipse, int min_ellipse, double min_dist);
  
  aImage = load_pbm_image(filename);

  double *matrix = (double *)malloc(20 * 5 * sizeof(double));
  matrix = find_ellipses_with_SHT(aImage, window_size, sample_proportion_for_local_max, min_local_max_value, min_ellipse_proportion, max_ellipse, min_ellipse, min_dist);
  return matrix;
}


int* matrix_of_imagePBM(char filename[])
{
  IMAGE   *aImage;
  IMAGE   *load_pbm_image(char filename[]); 
  aImage = load_pbm_image(filename);
  int x,y;
  int *matrix = (int *)malloc(aImage->x * aImage->y * sizeof(int));
  
  for (y=0; y < aImage->y; y++)
  {
      for (x=0; x < aImage->x; x++)
      {
          if (get_pixel_value(aImage,x,y) == BLACK)
            matrix[y*(aImage->x)+x] = 255;
          else
            matrix[y*(aImage->x)+x] = 0;
      }     
  }
  return matrix;
}

int* size_imagePBM(char filename[])
{
    int *s = (int *)malloc(2 * sizeof(int));
    FILE    *infile;
    char    magic_num[10];
    int     width, height;
    int     check_for_comment(FILE *infile);

    infile = fopen(filename, "r");
    if (infile == NULL)
        {
        fprintf(stderr, "Error opening file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }


    if (check_for_comment(infile) == EOF)
        {
        fprintf(stderr, "Error reading file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }

    if (fscanf(infile, "%[P14]", magic_num) != 1)
        {
        fprintf(stderr, "Error reading magic number of file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }

    if ((strlen(magic_num) != 2) ||
                            ((magic_num[1] != '1') && (magic_num[1] != '4')))
        {
        fprintf(stderr, "Error reading magic number of file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }

    if (check_for_comment(infile) == EOF)
        {
        fprintf(stderr, "Error reading file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }

    if (fscanf(infile, "%d", &width) != 1)
        {
        fprintf(stderr, "Error reading width in file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }

    if (check_for_comment(infile) == EOF)
        {
        fprintf(stderr, "Error reading file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }

    if (fscanf(infile, "%d", &height) != 1)
        {
        fprintf(stderr, "Error reading height in file %s in size_imagePBM() in image.c\n", filename);
        exit(1);
        }

  s[0] = height;
  s[1] = width;
  return s;
}

//gcc -lm -c -fPIC hough.c
//gcc -lm -shared hough.o -o hough.so

