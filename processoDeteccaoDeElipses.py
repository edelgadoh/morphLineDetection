# -*- coding: cp1252 -*-
from dissertacaoFuncoes import *

# Sample for script execution:
# python processoDeteccaoDeElipses.py '/home/user/folderWithImages' '/home/user/folderWithImagesResult' 150 0.1 150 0.3 500 50 0.95

theta = [
  0, 23,
 45, 68,
 90, 113,
135, 158
]

#Color names and RGB values with wide browser support
color = [
  'red','green','blue','aqua','blueviolet','chocolate','darkgreen','darkcyan','darkgoldenrod','dodgerblue',
  'goldenrod','gold','gray','indigo','khaki','lawngreen','lightsalmon','maroon','mediumpurple','slateblue'
]


def main(argv=None):
    if argv is None:
        argv = sys.argv

    imPathRead = argv[1]
    imPathSave = argv[2]
    windowSize = int(argv[3]) 
    sampleProportionForLocalMax = float(argv[4]) 
    minLocalMaxValue = int(argv[5]) 
    minEllipseProportion = float(argv[6]) 
    maxEllipse = int(argv[7]) 
    minEllipse = int(argv[8]) 
    minDist = float(argv[9]) 
    
    imSet      = imageSet3(idOS,imPathRead)
    
    
    if len(argv) != 10:
        print "Usage: " + argv[0] + " imPathRead imPathSave windowSize sampleProportionForLocalMax minLocalMaxValue minEllipseProportion maxEllipse minEllipse minDist"
        return "Invalid number of parameters"
    
    if os.access(imPathRead, os.F_OK) == False:
        return "\"" + argv[1] + "\" does not exist"
    
    if os.access(imPathSave, os.F_OK) == False:
        return "\"" + argv[2] + "\" does not exist"
    
    if os.access(imPathRead, os.R_OK) == False:
        return "Unable to open \"" + sys.argv[1] + "\""
    
    if os.access(imPathSave, os.R_OK) == False:
        return "Unable to open \"" + sys.argv[2] + "\""
    
    ini = datetime.now()
    for image in imSet:
       pathFile = fullPath(idOS,imPathRead,image)
       # read
       s = size_imagePBM(pathFile)
       m = matrix_of_imagePBM(pathFile,s[0,0],s[0,1])
       # process
       matrix = ellipse_hough_transform(pathFile,windowSize,sampleProportionForLocalMax,minLocalMaxValue,minEllipseProportion,maxEllipse,minEllipse,minDist)
       m = 255 - m 
       img = PIL.Image.fromarray(m)
       img = img.convert('RGB')
       # save
       imageName =  image[0:len(image)-4]
       pathFileOut = fullPathSaveOut3(idOS,imPathSave,imageName)
       
       for i in range(20):
         if matrix[i][0] != 0.0:
           img = draw_ellipse(matrix[i][0],matrix[i][1],matrix[i][2],matrix[i][3],matrix[i][4],img,color[i],pathFileOut)
         else:
           break
       img.save(pathFileOut,"PNG")
       print "File Created: ",pathFileOut
    
    fim = datetime.now()
    tempo = fim - ini
    print tempo


if __name__ == "__main__":
    sys.exit(main())
