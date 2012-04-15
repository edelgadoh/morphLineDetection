# -*- coding: cp1252 -*-
import pygtk
pygtk.require('2.0')
import gtk
import os
import sys
#import gobject

sys.path.append('/home/edelgado/Desktop/projeto/edge-detection-software/morph4python16')
os.putenv('LD_LIBRARY_PATH','$LD_LIBRARY_PATH:/home/edelgado/Desktop/projeto/edge-detection-software/morph4python16:/home/edelgado/Desktop/projeto/edge-detection-software')

#export PATH=$PATH:/home/edelgado/Desktop/projeto/edge-detection-software/morph4python16
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/edelgado/Desktop/projeto/edge-detection-software/morph4python16:/home/edelgado/Desktop/projeto/edge-detection-software


class MM_ImageProcessing:
    # Rank Filter process
    def exec_suavizado(self, widget, entry1, entry2, entry3, entry4, entry5):
        string1 = entry1.get_text()
        string2 = entry2.get_text()
        string3 = entry3.get_text()
        string4 = entry4.get_text()
        string5 = entry5.get_text()
        finalString = "python processoSuavizado.py '"+string1+"' '"+string2+"' "+string3+" "+string4+" "+string5
        os.system(finalString)
    
    def exec_searchFolder(self, widget, entry1):
	chooser = gtk.FileChooserDialog(title=None,action=gtk.FILE_CHOOSER_ACTION_OPEN,
                 buttons=(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN,gtk.RESPONSE_OK))
	chooser.set_action(gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER)
        response = chooser.run()
        if response == gtk.RESPONSE_OK:
            entry1.set_text(chooser.get_filename())
        elif response == gtk.RESPONSE_CANCEL:
            entry1.set_text('')
        chooser.destroy()
    
    # Rank Filter process
    def exec_rank_filter(self, widget, entry1, entry2, entry3, entry4, entry5):
        string1 = entry1.get_text()
        string2 = entry2.get_text()
        string3 = entry3.get_text()
        string4 = entry4.get_text()
        string5 = entry5.get_text()
        finalString = "python processoFiltroDeOrdem.py '"+string1+"' '"+string2+"' "+string3+" "+string4+" "+string5
        os.system(finalString)

    # Edge detection process
    def exec_edge_detection(self, widget, entry1, entry2, entry3, entry4, entry5,entry6):
        string1 = entry1.get_text()
        string2 = entry2.get_text()
        string3 = entry3.get_text()
        string4 = entry4.get_text()
        string5 = entry5.get_text()
        string6 = entry6.get_text()
        finalString = "python processoDeteccaoDeContornos.py '"+string1+"' '"+string2+"' "+string3+" "+string4+" "+string5+" "+string6
        os.system(finalString)

    # Ellipse detection process
    def exec_ellipse_detection(self, widget, entry1, entry2, entry3, entry6, entry7, entry8):
        string1 = entry1.get_text()
        string2 = entry2.get_text()
        string3 = entry3.get_text()
        #string4 = entry4.get_text()
        #string5 = entry5.get_text()
        string6 = entry6.get_text()
        string7 = entry7.get_text()
        string8 = entry8.get_text()
        #string9 = entry9.get_text()
	finalString =  "python processoDeteccaoDeElipses.py '"+string1+"' '"+string2+"' "+string3+" 1 80 "+string6+" "+string7+" "+string8+" 0.95"
        os.system(finalString)
    
    # This method rotates the position of the tabs
    def rotate_book(self, button, notebook):
        notebook.set_tab_pos((notebook.get_tab_pos()+1) %4)

    # Add/Remove the page tabs and the borders
    def tabsborder_book(self, button, notebook):
        tval = False
        bval = False
        if self.show_tabs == False:
	    tval = True 
        if self.show_border == False:
	    bval = True

        notebook.set_show_tabs(tval)
        self.show_tabs = tval
        notebook.set_show_border(bval)
        self.show_border = bval

    # Remove a page from the notebook
    def remove_book(self, button, notebook):
        page = notebook.get_current_page()
        notebook.remove_page(page)
        # Need to refresh the widget -- 
        # This forces the widget to redraw itself.
        notebook.queue_draw_area(0,0,-1,-1)

    def delete(self, widget, event=None):
        gtk.main_quit()
        return False

    def __init__(self):
        window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        window.connect("delete_event", self.delete)
        window.set_border_width(10)

        table = gtk.Table(3,6,False)
        window.add(table)

        # Create a new notebook, place the position of the tabs
        notebook = gtk.Notebook()
        notebook.set_tab_pos(gtk.POS_TOP)
        table.attach(notebook, 0,6,0,1)
        notebook.show()
        self.show_tabs = True
        self.show_border = True

        ### Page 0
        frame = gtk.Frame("Process parameters:")
        frame.set_border_width(10)
        frame.set_size_request(100, 375)
        
        box = gtk.VBox(False, 0)
        box.set_border_width(10)
        frame.add(box)
               
        entry1 = gtk.Entry()
        entry1.set_size_request(100, 20)
        box.pack_start(entry1,False,False,3)
        button = gtk.Button("Read Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry1)
        box.pack_start(button,False,False,3)        
        button.show()
        entry1.show()	

        entry2 = gtk.Entry()#entry.set_max_length(50)
        entry2.set_size_request(100, 20)
        box.pack_start(entry2,False,False,3)
	button = gtk.Button("Save Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry2)
        box.pack_start(button,False,False,3)        
        button.show()	        
	entry2.show()
	
        label = gtk.Label("Width of Window")
        box.pack_start(label,False,False,3)
        entry3 = gtk.Entry()
        entry3.set_max_length(3)
        entry3.set_text("8")
        entry3.set_size_request(3, 20)
        box.pack_start(entry3,False,False,3)
        label.show()
        entry3.show()

        label = gtk.Label("Height of Window")
        box.pack_start(label,False,False,3)
        entry4 = gtk.Entry()
        entry4.set_max_length(3)
        entry4.set_text("60")
        entry4.set_size_request(3, 20)
        box.pack_start(entry4,False,False,3)
        label.show()      
        entry4.show()

        label = gtk.Label("Width of inner window")
        box.pack_start(label,False,False,3)
        entry5 = gtk.Entry()
        entry5.set_max_length(3)
        entry5.set_text("30")
        entry5.set_size_request(3, 20)
        box.pack_start(entry5,False,False,3)
        label.show()      
        entry5.show()

        button = gtk.Button("Run")
        button.connect("clicked", self.exec_suavizado,entry1,entry2,entry3,entry4,entry5)
        box.pack_start(button,False,False,3)        
        button.show()

        box.show()
        frame.show()
        
        label = gtk.Label("Smoothing")
        notebook.insert_page(frame, label, 2)
        label.show()
        
        ### Page 1 
        frame = gtk.Frame("Process parameters:")
        frame.set_border_width(10)
        frame.set_size_request(100, 375)
        
        box = gtk.VBox(False, 0)
        box.set_border_width(10)
        frame.add(box)
        
        entry1 = gtk.Entry()
        entry1.set_size_request(100, 20)
        box.pack_start(entry1,False,False,3)
        button = gtk.Button("Read Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry1)
        box.pack_start(button,False,False,3)        
        button.show()
        entry1.show()	

        entry2 = gtk.Entry()#entry.set_max_length(50)
        entry2.set_size_request(100, 20)
        box.pack_start(entry2,False,False,3)
	button = gtk.Button("Save Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry2)
        box.pack_start(button,False,False,3)        
        button.show()	        
	entry2.show()

        label = gtk.Label("Rank window width")
        box.pack_start(label,False,False,3)
        entry3 = gtk.Entry()
        entry3.set_max_length(3)
        entry3.set_text("6")
        entry3.set_size_request(3, 20)
        box.pack_start(entry3,False,False,3)
        label.show()
        entry3.show()

        label = gtk.Label("Rank window heigth")
        box.pack_start(label,False,False,3)
        entry4 = gtk.Entry()
        entry4.set_max_length(3)
        entry4.set_text("1")
        entry4.set_size_request(3, 20)
        box.pack_start(entry4,False,False,3)
        label.show()      
        entry4.show()

        label = gtk.Label("Rank Value %")
        box.pack_start(label,False,False,3)
        entry5 = gtk.Entry()
        entry5.set_max_length(3)
        entry5.set_text("10")
        entry5.set_size_request(3, 20)
        box.pack_start(entry5,False,False,3)
        label.show()      
        entry5.show()

        button = gtk.Button("Run")
        button.connect("clicked", self.exec_rank_filter,entry1,entry2,entry3,entry4,entry5)
        box.pack_start(button,False,False,3)        
        button.show()

        box.show()
        frame.show()
        
        label = gtk.Label("Rank Filter")
        notebook.insert_page(frame, label, 2)
        label.show()

        ### Page 2 
        frame = gtk.Frame("Process parameters:")
        frame.set_border_width(10)
        frame.set_size_request(100, 375)
        
        box = gtk.VBox(False, 0)
        box.set_border_width(10)
        frame.add(box)
        
        entry1 = gtk.Entry()
        entry1.set_size_request(100, 20)
        box.pack_start(entry1,False,False,3)
        button = gtk.Button("Read Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry1)
        box.pack_start(button,False,False,3)        
        button.show()
        entry1.show()	

        entry2 = gtk.Entry()#entry.set_max_length(50)
        entry2.set_size_request(100, 20)
        box.pack_start(entry2,False,False,3)
	button = gtk.Button("Save Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry2)
        box.pack_start(button,False,False,3)        
        button.show()	        
	entry2.show()

        label = gtk.Label("Disk Ratio")
        box.pack_start(label,False,False,3)
        entry3 = gtk.Entry()
        entry3.set_max_length(2)
        entry3.set_text("7")
        entry3.set_size_request(3, 20)
        box.pack_start(entry3,False,False,3)
        label.show()
        entry3.show()

        label = gtk.Label("Length Line")
        box.pack_start(label,False,False,3)
        entry4 = gtk.Entry()
        entry4.set_max_length(4)
        entry4.set_text("72")
        entry4.set_size_request(3, 20)
        box.pack_start(entry4,False,False,3)
        label.show()      
        entry4.show()

        label = gtk.Label("Threshold")
        box.pack_start(label,False,False,3)
        entry5 = gtk.Entry()
        entry5.set_max_length(3)
        entry5.set_text("1")
        entry5.set_size_request(3, 20)
        box.pack_start(entry5,False,False,3)
        label.show()      
        entry5.show()

        label = gtk.Label("Area")
        box.pack_start(label,False,False,3)
        entry6 = gtk.Entry()
        entry6.set_max_length(4)
        entry6.set_text("600")
        entry6.set_size_request(3, 20)
        box.pack_start(entry6,False,False,3)
        label.show()      
        entry6.show()
      
        button = gtk.Button("Run")

        button.connect("clicked", self.exec_edge_detection,entry1,entry2,entry3,entry4,entry5,entry6)
        box.pack_start(button,False,False,3)        
        button.show()

        box.show()
        frame.show()
        
        label = gtk.Label("Edge Detection")
        notebook.insert_page(frame, label, 2)
        label.show()

        ### Page 3
        frame = gtk.Frame("Process parameters:")
        frame.set_border_width(10)
        frame.set_size_request(100, 500)
        
        box = gtk.VBox(False, 0)
        box.set_border_width(10)
        frame.add(box)
        
        entry1 = gtk.Entry()
        entry1.set_size_request(100, 20)
        box.pack_start(entry1,False,False,3)
        button = gtk.Button("Read Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry1)
        box.pack_start(button,False,False,3)        
        button.show()
        entry1.show()	

        entry2 = gtk.Entry()#entry.set_max_length(50)
        entry2.set_size_request(100, 20)
        box.pack_start(entry2,False,False,3)
	button = gtk.Button("Save Path Search ...")
        button.connect("clicked", self.exec_searchFolder,entry2)
        box.pack_start(button,False,False,3)        
        button.show()	        
	entry2.show()

        label = gtk.Label("Window size")
        box.pack_start(label,False,False,3)
        entry3 = gtk.Entry()
        entry3.set_max_length(4)
        entry3.set_text("80")
        entry3.set_size_request(3, 20)
        box.pack_start(entry3,False,False,3)
        label.show()
        entry3.show()

        #label = gtk.Label("Sample proportion for local max")
        #box.pack_start(label,False,False,3)
        #entry4 = gtk.Entry()
        #entry4.set_max_length(4)
        #entry4.set_text("1")
        #entry4.set_size_request(3, 20)
        #box.pack_start(entry4,False,False,3)
        #label.show()      
        #entry4.show()
        #label = gtk.Label("Min Local Max Value")
        #box.pack_start(label,False,False,3)
        #entry5 = gtk.Entry()
        #entry5.set_max_length(4)
        #entry5.set_text("80")
        #entry5.set_size_request(3, 20)
        #box.pack_start(entry5,False,False,3)
        #label.show()      
        #entry5.show()
        label = gtk.Label("Min ellipse proportion")
        box.pack_start(label,False,False,3)
        entry6 = gtk.Entry()
        entry6.set_max_length(4)
        entry6.set_text("0.25")
        entry6.set_size_request(3, 20)
        box.pack_start(entry6,False,False,3)
        label.show()      
        entry6.show()

        label = gtk.Label("Max length ellipse")
        box.pack_start(label,False,False,3)
        entry7 = gtk.Entry()
        entry7.set_max_length(4)
        entry7.set_text("500")
        entry7.set_size_request(3, 20)
        box.pack_start(entry7,False,False,3)
        label.show()      
        entry7.show()
	
	label = gtk.Label("Min length ellipse")
        box.pack_start(label,False,False,3)
        entry8 = gtk.Entry()
        entry8.set_max_length(4)
        entry8.set_text("110")
        entry8.set_size_request(3, 20)
        box.pack_start(entry8,False,False,3)
        label.show()      
        entry8.show()
	
        #label = gtk.Label("Min Distance")
        #box.pack_start(label,False,False,3)
        #entry9 = gtk.Entry()
        #entry9.set_max_length(4)
        #entry9.set_text("0.95")
        #entry9.set_size_request(3, 20)
        #box.pack_start(entry9,False,False,3)
        #label.show()      
        #entry9.show()
      
        button = gtk.Button("Run")

        button.connect("clicked", self.exec_ellipse_detection,entry1,entry2,entry3,entry6,entry7,entry8)
        box.pack_start(button,False,False,3)        
        button.show()

        box.show()
        frame.show()
        
        label = gtk.Label("Ellipse Detection")
        notebook.insert_page(frame, label, 3)
        label.show()

        
        # Set what page to start at (page 4)
        notebook.set_current_page(0)

        # Create a bunch of buttons
        button = gtk.Button("next page")
        button.connect("clicked", lambda w: notebook.next_page())
        table.attach(button, 0,1,1,2)
        button.show()

        button = gtk.Button("prev page")
        button.connect("clicked", lambda w: notebook.prev_page())
        table.attach(button, 1,2,1,2)
        button.show()

        button = gtk.Button("tab position")
        button.connect("clicked", self.rotate_book, notebook)
        table.attach(button, 2,3,1,2)
        button.show()

        button = gtk.Button("close")
        button.connect("clicked", self.delete)
        table.attach(button, 3,4,1,2)
        button.show()

        table.show()
        window.show()


def main():
    gtk.main()
    return 0

if __name__ == "__main__":
    MM_ImageProcessing()
    main()

