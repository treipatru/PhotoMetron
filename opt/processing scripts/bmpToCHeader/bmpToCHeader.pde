//--------------------------------------------------------//
//  Bitmap Image Convert Directory //Processing3
//  
//  Converts images to C header file suitable for printing 
//  with the Adafruit Thermal Printer.
//
//  This script is a modified version of the 
//  bitmapImageConvert that ships with Thermal Printer 
//  library. It will batch process an entire folder at a  
//  time instead of single images.
//  
//  andrei@planet34.org
//--------------------------------------------------------//





//********************************************************
//Define variables for creating directory listing array
String     sourcedir;
String[]   children;
int        childCount = 0;
File dir;


//********************************************************
//Define variables for processing an image
String      fileName;           //Filename+ext
String      baseName;           //Filename
String      fileNameBin;        //
String      fullPath;           //Path+filename
String      fileExt = ".bmp";   //Original images extension
PImage      img;
PrintWriter output;
int         pixelNum, byteNum, bytesOnLine = 99, 
  x, y, n, b, rowBytes, totalBytes, lastBit, sum;

byte[] data;
int byteIndex = 0;


//--------------------------------------------------------//
//
// SETUP
//
//--------------------------------------------------------//





void setup() {
  //********************************************************
  //Define directory to process
  sourcedir = ((System.getProperty("user.home"))+"\\Pictures\\Export\\arduinotest");
  dir = new File(sourcedir);
  println ("Processing " + dir);

  children = dir.list();



  //********************************************************
  //Do something for every item in the directory from above   
  for (int i = 0; i<children.length; i++) {
    //Update the fileName
    fileName = children[i];

    //Update the baseName
    x = fileName.lastIndexOf('.');
    baseName = fileName.substring(0, x);

    //Update the fullPath
    fullPath = (sourcedir + '\\' + baseName);

    //Load the file, stop the program
    img = loadImage(fullPath + fileExt);

    fileNameBin = fullPath+".bin";
    println("Output to " + fileName);

    // Calculate output size
    rowBytes   = (img.width + 7) / 8;
    totalBytes = rowBytes * img.height;

    //Create array for bytes
    data = new byte[totalBytes];

    // Convert image to B&W, make pixels readable
    img.filter(THRESHOLD);
    img.loadPixels();

    // Generate body of array
    for (i=n=y=0; y<img.height; y++) { // Each row...
      //    output.print("\n  ");
      for (x=0; x<rowBytes; x++) { // Each 8-pixel block within row...
        lastBit = (x < rowBytes - 1) ? 1 : (1 << (rowBytes * 8 - img.width));
        sum     = 0; // Clear accumulated 8 bits
        for (b=128; b>=lastBit; b >>= 1) { // Each pixel within block...
          if ((img.pixels[i++] & 1) == 0) sum |= b; // If black pixel, set bit
        }      

        //Write to byte array
        data[byteIndex] = byte(sum);
        if (byteIndex > 0) {
          data[byteIndex] = byte(data[byteIndex-1] << 8 | data[byteIndex]);
          //                println("byteIndex "+(byteIndex-1)+": "+data[byteIndex-1]);
        }
        byteIndex++;
      }
    }

    // save byte array as binary file
    saveBytes(fileNameBin, data);
    println(fileNameBin + " done!");
  }

  exit();
}