# -*- coding: cp1252 -*-
from dissertacaoFuncoes import *

# Sample for script execution:
# python processoDeteccaoDeContornos.py '/home/user/folderWithImages/' '/home/user/folderWithImagesResult/' 6 70 1 500

theta = [
  0,  5, 11, 16, 22, 27, 33, 38,
 45, 50, 56, 61, 66, 71, 77, 82,
 90, 95,101,106,112,117,123,128,
135,140,146,151,157,162,168,173
]

def main(argv=None):
    if argv is None:
        argv = sys.argv

    imPathRead = argv[1]
    imPathSave = argv[2]
    seDisk = mmsedisk(int(argv[3])) 
    lengthLine = int(argv[4]) 
    threshold = int(argv[5]) 
    area = int(argv[6]) 
    
    imSet      = imageSet(idOS,imPathRead)
    
    if len(argv) != 7:
        print "Usage: " + argv[0] + " imPathRead imPathSave seDisk(x) lengthLine threshold area"
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
       f = aImg
       f = mmcloseth(f,seDisk)
       #imageName =  image[0:len(image)-4]+"_1"
       #aFinal = array(f,dtype=uint8)
       #imageOut = PIL.Image.fromarray(aFinal) # o parametro deve ser um arranjo
       #pathFileOut = fullPathSaveOut3(idOS,imPathSave,imageName) # construcao de pathFileOut
       #imageOut.save(pathFileOut,"PNG")
       #print "File Created 1: ",pathFileOut
       f = lineErosion(f,lengthLine,theta)
       #imageName =  image[0:len(image)-4]+"_2"
       #aFinal = array(mmneg(f),dtype=uint8)
       #imageOut = PIL.Image.fromarray(aFinal) # o parametro deve ser um arranjo
       #pathFileOut = fullPathSaveOut3(idOS,imPathSave,imageName) # construcao de pathFileOut
       #imageOut.save(pathFileOut,"PNG")
       #print "File Created 2: ",pathFileOut
       #imageName =  image[0:len(image)-4]+"_2_2"
       #aFinal = array(f,dtype=uint8)
       #imageOut = PIL.Image.fromarray(aFinal) # o parametro deve ser um arranjo
       #pathFileOut = fullPathSaveOut3(idOS,imPathSave,imageName) # construcao de pathFileOut
       #imageOut.save(pathFileOut,"PNG")
       #print "File Created 2: ",pathFileOut
       f = mmthreshad(f,threshold)
       #imageName =  image[0:len(image)-4]+"_3"
       #aFinal = array(mmgray(mmneg(f)),dtype=uint8)
       #imageOut = PIL.Image.fromarray(aFinal) # o parametro deve ser um arranjo
       #pathFileOut = fullPathSaveOut3(idOS,imPathSave,imageName) # construcao de pathFileOut
       #imageOut.save(pathFileOut,"PNG")
       #print "File Created 3: ",pathFileOut
       f = mmareaopen(f,area)
       #imageName =  image[0:len(image)-4]+"_4"
       #aFinal = array(mmgray(mmneg(f)),dtype=uint8)
       #imageOut = PIL.Image.fromarray(aFinal) # o parametro deve ser um arranjo
       #pathFileOut = fullPathSaveOut3(idOS,imPathSave,imageName) # construcao de pathFileOut
       #imageOut.save(pathFileOut,"PNG")
       #print "File Created 4: ",pathFileOut
       f = mmedgeoff(f)
       #imageName =  image[0:len(image)-4]+"_5"
       #aFinal = array(mmgray(mmneg(f)),dtype=uint8)
       #imageOut = PIL.Image.fromarray(aFinal) # o parametro deve ser um arranjo
       #pathFileOut = fullPathSaveOut3(idOS,imPathSave,imageName) # construcao de pathFileOut
       #imageOut.save(pathFileOut,"PNG")
       #print "File Created 5: ",pathFileOut
       # salvar
       imageName =  image[0:len(image)-4]
       aFinal = array(f,dtype=uint8)
       imageOut = PIL.Image.fromarray(aFinal) 
       xsize, ysize = imageOut.size
       imageOut = imageOut.resize((xsize/3,ysize/3))
       pathFileOut = fullPathSaveOut2(idOS,imPathSave,imageName) 
       savePBM(pathFileOut,imageOut)
       print "File Created: ",pathFileOut    
    
    fim = datetime.now()
    tempo = fim - ini
    print tempo


if __name__ == "__main__":
    sys.exit(main())
