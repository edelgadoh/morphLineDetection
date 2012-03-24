# -*- coding: cp1252 -*-
# export PATH=$PATH:/home/edelgado/Desktop/projeto/morph4python16
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/edelgado/Desktop/projeto/morph4python16:/home/edelgado/Desktop/projeto
# Para executar o presente script ubicarse com cd até a pasta projeto
import sys
import os
import glob
import PIL.Image
import ImageDraw
from morph4python16.morph import *
from datetime import datetime
from numpy import *
from ctypes import *

# inicialização
#export PATH=$PATH:/home/edelgado/Desktop/projeto/edge-detection-software
#LD_LIBRARY_PATH used for sys
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/edelgado/Desktop/projeto/edge-detection-software
#sys.path.append('/home/edelgado/Desktop/projeto/edge-detection-software')
#os.putenv('LD_LIBRARY_PATH','$LD_LIBRARY_PATH:/home/edelgado/Desktop/projeto/morph4python16:/home/edelgado/Desktop/projeto')
#os.putenv('PATH','$PATH:/home/edelgado/Desktop/projeto/edge-detection-software')
#os.putenv('LD_LIBRARY_PATH','$LD_LIBRARY_PATH:/home/edelgado/Desktop/projeto/edge-detection-software')
# pesquisar a função para automaticamente recuperar o sistema operativo
idOS = 1 # 0 - windows, 1 - linux

scriptPath = os.getcwd() #'/home/edelgado/Desktop/projeto'

# funções
def imageSet(idOS,imagePath):
    os.chdir(imagePath) # pasta onde estão nossas imagens tif
    images = glob.glob('*.tif')
    if idOS == 1:
      os.chdir(scriptPath) # para o correto uso da biblioteca morph4python16.morph
    return images

def imageSet2(idOS,imagePath):
    os.chdir(imagePath) # pasta onde estão nossas imagens ppm
    images = glob.glob('*.ppm')
    if idOS == 1:
      os.chdir(scriptPath) # para o correto uso da biblioteca morph4python16.morph
    return images

def imageSet3(idOS,imagePath):
    os.chdir(imagePath) # pasta onde estão nossas imagens pbm
    images = glob.glob('*.pbm')
    if idOS == 1:
      os.chdir(scriptPath) # para o correto uso da biblioteca morph4python16.morph
    return images

def fullPath(idOS,imagePath,image):
    if idOS == 1:
      file = imagePath+'/'+image
    else:
      file = imagePath+'\\'+image  
    return file

def fullPathSaveOutTIF(idOS,imagePath,image):
    if idOS == 1:
      file = os.path.splitext(imagePath+'/'+image)[0] + "-out.tif"
    else:
      file = os.path.splitext(imagePath+'\\'+image)[0] + "-out.tif"
    return file

def fullPathSaveOut(idOS,imagePath,image):
    if idOS == 1:
      file = os.path.splitext(imagePath+'/'+image)[0] + "-rf.tif" #"-out.ppm"
    else:
      file = os.path.splitext(imagePath+'\\'+image)[0] + "-rf.tif" #"-out.ppm"
    return file

def fullPathSaveOut2(idOS,imagePath,image):
    if idOS == 1:
      file = os.path.splitext(imagePath+'/'+image)[0] + "-edge.pbm"
    else:
      file = os.path.splitext(imagePath+'\\'+image)[0] + "-edge.pbm"
    return file

def fullPathSaveOut3(idOS,imagePath,image):
    if idOS == 1:
      file = os.path.splitext(imagePath+'/'+image)[0] + "-ellipse.png"
    else:
      file = os.path.splitext(imagePath+'\\'+image)[0] + "-ellipse.png"
    return file

def savePBM(pathFileOut,image): #Função para salvar PBM
    file = open(pathFileOut,'w')
    pix = image.load()
    print >>file, "P1"
    print >>file, "%d %d" % image.size
    #print >>file, "255"
    for y in range(image.size[1]):
    	for x in range(image.size[0]):
    	  p = pix[x,y]
    	  print >>file, p
    	print >>file
    file.close()    
    return pathFileOut
    
def seLine(length,width,theta):
    # Construcao do elemento estruturante
    ratio = length/2
    seLine1 = mmseline(ratio,theta)
    seLine2 = mmseline(ratio,theta+180)
    seLine  = mmseunion(seLine1,seLine2)
    seLine = mmsedil(seLine,mmsedisk(round(width/2)))
    line = mmseshow(seLine)
    line = array(line,dtype=uint8)
    # Tamanho da janela
    if theta in (0,180) and width == 1:
      x = 1
      y = line.shape[0]
    else:
      x = line.shape[0]
      y = line.shape[1]
    return seLine,line,x,y

def seBreakLine(length1,length2,width,theta):
    seLineA,lineA,xA,yA = seLine(length1,width,theta)
    seLineB,lineB,xB,yB = seLine(length2,width,theta)
    lineA2 = array(zeros((xA,yA)),dtype=uint8)
    dxA = xA/2
    dyA = yA/2
    dxB = xB/2
    dyB = yB/2
    for i in range(0,xB):
      for j in range(0,yB):
        lineA[dxA-dxB+i,dyA-dyB+j] = lineA[dxA-dxB+i,dyA-dyB+j] - lineB[i,j]
        if lineB[i,j] == 1:
          lineA2[dxA-dxB+i,dyA-dyB+j] = 1
      
    if theta in (0,180) and width == 1:
      x = 1
      y = lineA.shape[0]
    else:
      x = lineA.shape[0]
      y = lineA.shape[1]
    breakLine = lineA
    seBreakLine = mmimg2se(mmbinary(breakLine))
    return seBreakLine,breakLine,x,y,lineA2

def lineErosion(image,length,theta):
    x  = image.shape[0]
    y  = image.shape[1]
    im = zeros((x,y),dtype=uint8)
    for i in range(len(theta)):
      se01  = mmseline(length,theta[i])
      ero01 = mmero(image,se01)
      im = mmunion(im,ero01)
    
    return im

def lineOpen(image,length,theta):
    x  = image.shape[0]
    y  = image.shape[1]
    im = zeros((x,y))
    for i in range(len(theta)):
      se01  = mmseline(length,theta[i])
      se02  = mmseline(length/3,theta[i])
      erosao = mmero(image,se01)
      open = mmdil(erosao,se02)
      im = mmunion(im,open)
    
    return im

def lineCloseTH(image,length,theta):
    x  = image.shape[0]
    y  = image.shape[1]
    im = zeros((x,y))
    for i in range(len(theta)):
      se01  = mmseline(length,theta[i])
      fth = mmcloseth(image,se01)
      im = mmunion(im,fth)
    
    return im

def lineDilation(image,length,theta):
    x  = image.shape[0]
    y  = image.shape[1]
    im = zeros((x,y))
    for i in range(len(theta)):
      se01  = mmseline(length,theta[i])
      dil = mmdil(image,se01)
      im = mmunion(im,dil)
    
    return im

def rankFilterX(img,length,width,theta,pRank=20):      
    so = CDLL(scriptPath+"/myFunctions.so")
    so.rankFilter.argtypes= [POINTER(c_int),c_int,c_int,c_int,c_int,c_int,POINTER(c_int),c_int]
    so.rankFilter.restype = POINTER(c_int)
    
    seline,line,x,y = seLine(length,width,theta)
    # Indices que sÃ£o de interesse
    P = [] # lista das posicoes vÃ¡lidas
    line2 = line.ravel()
    for i in range(0,size(line2)):
      if line2[i] == 1:
        P.append(i)
    # Filtro de ordem --- P.append(y*i+j)
    xsize = img.shape[0] # tamanho da altura da imagem
    ysize = img.shape[1] # tamanho da largura da imagem
    dx = x/2 # metade da altura da janela
    dy = y/2 # metade da largura da janela
    aImg = asarray(img)
    aImg1 = aImg.copy()
    aImg2 = aImg.ravel()
    
    INT = c_int
    INT_P_ARR = INT * len(P)
    INT_A_ARR = INT * len(aImg2)
    p = INT_P_ARR()
    ptr  = INT_A_ARR()
    ptr2 = INT_A_ARR()
    for i in range(len(P)):
        p[i] = P[i]
    for i in range(len(aImg2)):
        ptr[i] = aImg2[i]
    for i in range(len(aImg2)):
        ptr2[i] = 0
    ptr2 = so.rankFilter(ptr,x,y,xsize,ysize,pRank,p,len(P))
    aImg3 = array(zeros((xsize*ysize,)),dtype=uint8)
    for i in range(len(aImg3)):
        aImg3[i] = ptr2[i]
    aImg3.shape = (xsize, ysize)
    return aImg3


def rankFilter(img,length,width,theta,pRank=20):      
    # Construcao do elemento estruturante
    seline,line,x,y = seLine(length,width,theta)
    # Indices que são de interesse
    P = [] # lista das posicoes válidas
    line2 = line.ravel()
    for i in range(0,size(line2)):
      if line2[i] == 1:
        P.append(i)
    # Filtro de ordem --- P.append(y*i+j)
    xsize = img.shape[0] # tamanho da altura da imagem
    ysize = img.shape[1] # tamanho da largura da imagem
    dx = x/2 # metade da altura da janela
    dy = y/2 # metade da largura da janela
    aImg = asarray(img)
    aImg1 = aImg.copy()  
    for i in range(dx,xsize-dx-1):
      for j in range(dy,ysize-dy-1):
        W = aImg[i-dx:i+dx+1,j-dy:j+dy+1]
        L = W.ravel()
        B = [] # lista dos níveis de cinza válidos
        for p in P:
          B.append(L[p])
        C = sort(B)
        rank1 = round((len(C)-1)*pRank/100)
        val1 = C[rank1]
        aImg1[i][j] = val1
    return aImg1


def suavizado(img,length1,length2,width,theta):
    # Construcao do elemento estruturante
    seline,line,x,y,line2 = seBreakLine(length1,length2,width,theta)
    # Indices que são de interesse
    # implementar uma matriz que armazene valores medios entre dois extremos
    xsize = img.shape[0] # tamanho da altura da imagem
    ysize = img.shape[1] # tamanho da largura da imagem
    dx = x/2 # metade da altura da janela
    dy = y/2 # metade da largura da janela
    P1 = [] # lista das posicoes válidas grupo 1
    P2 = [] # lista das posicoes válidas grupo 2
    P3 = [] # lista das posicoes válidas grupo 3
    #line2 = line.ravel()
    #temp
    dist = round((length1/2 + length2)/2)
    i1 = int(round(dx+dist*sin(theta*pi/180)))
    j1 = int(round(dy+dist*cos(theta*pi/180)))
    i2 = int(round(dx-dist*sin(theta*pi/180)))
    j2 = int(round(dy-dist*cos(theta*pi/180)))
    P1 = agrupar(P1,line,x,y,i1,j1)
    P2 = agrupar(P2,line,x,y,i2,j2)
    P3 = agrupar(P3,line2,x,y,dx,dy)
    print "Tamanho para theta:"+str(theta)
    print len(P1)
    print len(P2)
    print len(P3)
    print line
    print line2
    #fin de calculo de grupos
    aImg = asarray(img)
    aImg1 = aImg.copy()  
    for i in range(dx,xsize-dx-1,dx):
      for j in range(dy,ysize-dy-1,dy):
        W = aImg[i-dx:i+dx+1,j-dy:j+dy+1]
        L = W.ravel()
        B1 = [] # lista dos níveis de cinza válidos grupo 1
        B2 = [] # lista dos níveis de cinza válidos grupo 2
        B3 = [] # lista dos níveis de cinza válidos grupo 3 a ser preenchido
        n1 = 0
        n2 = 0
        n3 = 0
        n4 = 0
        n5 = 0
        n6 = 0
        minIntensity = 255
        maxIntensity = 0
        for p in P1:
          B1.append(L[p])
          if L[p] <= 60:
            n1 = n1 + 1
            if L[p] < minIntensity:
              minIntensity = L[p]
          if L[p] >= 190:
            n4 = n4 + 1
            if L[p] > maxIntensity:
              maxIntensity = L[p]
        for p in P2:
          B2.append(L[p])
          if L[p] <= 60:
            n2 = n2 + 1
            if L[p] < minIntensity:
              minIntensity = L[p]
          if L[p] >= 190:
            n5 = n5 + 1
            if L[p] > maxIntensity:
              maxIntensity = L[p]              
        for p in P3:
          B3.append(L[p])
          if L[p] <= 60:
            n3 = n3 + 1
          if L[p] >= 190:
            n6 = n6 + 1
        
        #logica para trabalhar grupos
        avg1 = sum(B1)/len(B1)
        avg2 = sum(B2)/len(B2)
        avg3 = sum(B3)/len(B3)
        cv1 = std(B1)/avg1
        cv2 = std(B2)/avg2
        cv3 = std(B3)/avg3
        minAVG = min(min(avg1,avg2),min(avg2,avg3))
        maxAVG = max(max(avg1,avg2),max(avg2,avg3))
        if cv1 <= 0.4 or cv2 <= 0.4 or cv3 <= 0.4 and 1==2:
          i0 = i-dx
          j0 = j-dy
          for p in P3:
            iAux = p/y
            jAux = p%y
            aImg1[i0+iAux][j0+jAux] = maxAVG
          for p in P1:
            iAux = p/y
            jAux = p%y
            aImg1[i0+iAux][j0+jAux] = maxAVG
          for p in P2:
            iAux = p/y
            jAux = p%y
            aImg1[i0+iAux][j0+jAux] = maxAVG
        
        difAVG = 80 #10 degradou muito as linhas escuras, 25 fiz um suavizado considerável , 50 ok
        if abs(avg1-avg3) >= difAVG and abs(avg2-avg3) >= difAVG and abs(avg1-avg2) <= difAVG: #25
          i0 = i-dx
          j0 = j-dy
          for p in P3:
            iAux = p/y
            jAux = p%y
            aImg1[i0+iAux][j0+jAux] = minAVG
          #for p in P1:
          #  iAux = p/y
          #  jAux = p%y
          #  aImg1[i0+iAux][j0+jAux] = minAVG
          #for p in P2:
          #  iAux = p/y
          #  jAux = p%y
          #  aImg1[i0+iAux][j0+jAux] = minAVG
            
    return aImg1


def agrupar(grupo,matriz,x,y,i,j):
    index = y*i+j
    if (index in grupo) == False and matriz[i,j] == 1:
      grupo.append(index)
      if i == 0 and j == 0:
        agrupar(grupo,matriz,x,y,i,j+1)
        agrupar(grupo,matriz,x,y,i+1,j)
      elif i == 0 and j == y-1:
        agrupar(grupo,matriz,x,y,i,j-1)
        agrupar(grupo,matriz,x,y,i+1,j)
      elif i == x-1 and j == 0:
        agrupar(grupo,matriz,x,y,i,j+1)
        agrupar(grupo,matriz,x,y,i-1,j)
      elif i == x-1 and j == y-1:
        agrupar(grupo,matriz,x,y,i,j-1)
        agrupar(grupo,matriz,x,y,i-1,j)
      elif i == 0:
        agrupar(grupo,matriz,x,y,i,j-1)
        agrupar(grupo,matriz,x,y,i,j+1)
        agrupar(grupo,matriz,x,y,i+1,j)
      elif j == 0:
        agrupar(grupo,matriz,x,y,i,j+1)
        agrupar(grupo,matriz,x,y,i-1,j)
        agrupar(grupo,matriz,x,y,i+1,j)
      elif i == x-1:
        agrupar(grupo,matriz,x,y,i,j-1)
        agrupar(grupo,matriz,x,y,i,j+1)
        agrupar(grupo,matriz,x,y,i-1,j)
      elif j == y-1:
        agrupar(grupo,matriz,x,y,i,j-1)
        agrupar(grupo,matriz,x,y,i-1,j)
        agrupar(grupo,matriz,x,y,i+1,j)
      else:
        agrupar(grupo,matriz,x,y,i,j-1)
        agrupar(grupo,matriz,x,y,i,j+1)
        agrupar(grupo,matriz,x,y,i-1,j)
        agrupar(grupo,matriz,x,y,i+1,j)
    return grupo  


#WINDOW_SIZE = 150 # tamanho da janela para analisar o histograma, valor original 50, valor proposto 150
#SAMPLE_PROPORTION_FOR_LOCAL_MAX = 0.1 # porcentagem que é multiplicado pelo tamanho da janela para realizar testes em parâmetros, valor original 0.1, valor proposto 0.1
#MIN_LOCAL_MAX_VALUE = 150 # quantidade mínima de pontos acumulados no histograma para considerá-lo um ponto candidato, valor original 4500, valor proposto 150
#MIN_ELLIPSE_PROPORTION = 0.3 # proporção mínima de uma elipse tolerável. valor original 0.65, valor proposto 0.3
#MAX_ELLIPSE = 500 # tamanho máximo de uma ellipse na imagem = 2*a, onde 'a' é o eixo maior, valor original diagonal do tamanho da imagem, valor proposto 500
#MIN_ELLIPSE = 50 # tamanho mínimo de uma ellipse na imagem = 2*b, onde 'b' é o eixo menor, valor proposto 50
#MIN_DIST = 0.95 # diferença mínima tolerável entre o centro da elipse a um candidato a ponto, valor original 0.45, valor proposto 0.95 

def ellipse_hough_transform(path,ws,spflm,mlmv,mep,me,minElipse,md):      
    so = CDLL(scriptPath+"/hough.so")
    so.ellipse_hough_transform.argtypes= [c_char_p,c_int,c_double,c_int,c_double,c_int,c_int,c_double]
    so.ellipse_hough_transform.restype = POINTER(c_double)
    
    DOUBLE = c_double
    DOUBLE_A_ARR = DOUBLE * (20*5)
    ptr2 = DOUBLE_A_ARR()
    for i in range(20*5):
        ptr2[i] = 0
    ptr2 = so.ellipse_hough_transform(path,ws,spflm,mlmv,mep,me,minElipse,md)
    aImg3 = array(zeros((20*5,)),dtype=float)
    for i in range(len(aImg3)):
        aImg3[i] = ptr2[i]
    aImg3.shape = (20, 5)
    return aImg3


def matrix_of_imagePBM(path,xsize,ysize):      
    so = CDLL(scriptPath+"/hough.so")
    so.matrix_of_imagePBM.argtypes= [c_char_p]
    so.matrix_of_imagePBM.restype = POINTER(c_int)
    INT = c_int
    INT_A_ARR = INT * (xsize*ysize)
    ptr2 = INT_A_ARR()
    for i in range(xsize*ysize):
        ptr2[i] = 0
    ptr2 = so.matrix_of_imagePBM(path)
    aImg3 = array(zeros((xsize*ysize,)),dtype=uint8)
    for i in range(len(aImg3)):
        aImg3[i] = ptr2[i]
    aImg3.shape = (xsize, ysize)
    return aImg3


def size_imagePBM(path):
    so = CDLL(scriptPath+"/hough.so")
    so.size_imagePBM.argtypes= [c_char_p]
    so.size_imagePBM.restype = POINTER(c_int)
    INT = c_int
    INT_A_ARR = INT * (2)
    ptr2 = INT_A_ARR()
    for i in range(2):
        ptr2[i] = 0
    ptr2 = so.size_imagePBM(path)
    aImg3 = array(zeros((2,)),dtype=int)
    for i in range(len(aImg3)):
        aImg3[i] = ptr2[i]
    aImg3.shape = (1, 2)
    return aImg3


def draw_ellipse(x,y,r1,r2,theta,img,color,pathFileOut):      
    draw = ImageDraw.Draw(img)
    t_1 = r1*(cos(theta))**2 + r2*(sin(theta))**2
    t_2 = r1*sin(theta)*cos(theta) - r2*sin(theta)*cos(theta)
    t_3 = t_2
    t_4 = r1*(sin(theta))**2 + r2*(cos(theta))**2
    
    x_old1 = t_1
    x_old2 = t_2
    x_old3 = t_3
    x_old4 = t_4
    
    x_original1 = x_old1 = x + x_old1
    x_original2 = x_old2 = y + x_old2
    
    p = arange(0.0,2.0*pi,0.4)
    
    for t in p:
      t1 = cos(t)
      t2 = sin(t)
      x1 = t_1*t1 + t_2*t2
      x2 = t_3*t1 + t_4*t2
      x1 = x + x1
      x2 = y + x2
      draw.line((x_old1,x_old2,x1,x2),width=3, fill=color)
      x_old1 = x1
      x_old2 = x2
    
    draw.line((x_original1,x_original2,x1,x2),width=3, fill=color)
    #del draw 
    #savePBM(pathFileOut,img)
    return img



