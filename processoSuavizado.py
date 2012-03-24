# -*- coding: cp1252 -*-
from dissertacaoFuncoes import *

# Sample for script execution:
# python processoSuavizado.py /home/user/folderWithImages/ /home/user/folderWithImagesResult/ 8 60 30


theta = [
  0, 23,
  45, 68, 
 113,90,
 135, 158
]

def main(argv=None):
    if argv is None:
        argv = sys.argv
    
    if len(argv) != 6:
        print "Usage: " + argv[0] + " imPathRead imPathSave widthWindow heightWindow heightInnerWindow"
        return "Invalid number of parameters"
    
    imPathRead = argv[1]
    imPathSave = argv[2]
    widthWindow = int(argv[3])
    heightWindow = int(argv[4])
    heightInnerWindow = int(argv[5])
    
    imSet = imageSet(idOS,imPathRead);
    
    
    if len(argv) != 6:
        print "Usage: " + argv[0] + " imPathRead imPathSave widthWindow heightWindow heightInnerWindow"
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
       try:
          img = PIL.Image.open(pathFile) 
       except Exception as e:
          return "\"" + pathFile + "\": " + str(e)
       
       img = img.convert("L")
       aImg = asarray(img)
       
       # process
       x = aImg.shape[0]
       y = aImg.shape[1]
       f01 = aImg
       for i in range(len(theta)):
         f1 = suavizado(aImg,heightWindow,heightInnerWindow,widthWindow,theta[i]) 
         f01 = mmintersec(f01,f1)
       
       # save
       imageName =  image[0:len(image)-4]
       aFinal = array(f01,dtype=uint8)
       imageOut = PIL.Image.fromarray(aFinal) 
       pathFileOut = fullPathSaveOutTIF(idOS,imPathSave,imageName+'-suavizado') 
       imageOut.save(pathFileOut,"TIFF")
       print "File Created: ",pathFileOut
    
    fim = datetime.now()
    tempo = fim - ini
    sys.stdout.write('\a')
    sys.stdout.flush()
    print tempo


if __name__ == "__main__":
    sys.exit(main())

