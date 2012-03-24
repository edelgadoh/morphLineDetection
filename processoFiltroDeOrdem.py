# -*- coding: cp1252 -*-
from dissertacaoFuncoes import *

# Sample for script execution:
# python processoFiltroDeOrdem.py '/home/user/folderWithImages/' '/home/user/folderWithImagesResult/' 6 1 10

theta = [
  0, 23,
 45, 68,
 90, 113,
135, 158
]

def main(argv=None):
    if argv is None:
        argv = sys.argv

    imPathRead = argv[1]
    imPathSave = argv[2]
    widthWindowRank = int(argv[3]) 
    heightWindowRank = int(argv[4]) 
    pRank = int(argv[5]) 
    
    imSet      = imageSet(idOS,imPathRead)
    
    if len(argv) != 6:
        print "Usage: " + argv[0] + " imPathRead imPathSave widthWindowRank heightWindowRank pRank"
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
         f1  = rankFilterX(aImg,widthWindowRank,heightWindowRank,theta[i],pRank)
         f01 = mmintersec(f01,f1) 
       
       # save
       imageName =  image[0:len(image)-4]
       aFinal = array(f01,dtype=uint8)
       imageOut = PIL.Image.fromarray(aFinal) 
       pathFileOut = fullPathSaveOut(idOS,imPathSave,imageName) 
       imageOut.save(pathFileOut,"TIFF") 
       print "File Created: ",pathFileOut
    
    fim = datetime.now()
    tempo = fim - ini
    sys.stdout.write('\a')
    sys.stdout.flush()
    print tempo


if __name__ == "__main__":
    sys.exit(main())

