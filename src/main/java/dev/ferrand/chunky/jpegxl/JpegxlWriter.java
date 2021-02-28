package dev.ferrand.chunky.jpegxl;

import se.llbit.chunky.resources.BitmapImage;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URL;

public class JpegxlWriter {

  private static boolean isWindows()
  {
    return System.getProperty("os.name").toLowerCase().contains("win");
  }

  static {
    String libName;
    // TODO Support other OS
    if(isWindows())
      libName = "jpegxl_writer.dll";
    else
      libName = "libjpegxl_writer.so";
    URL libLocation = JpegxlWriter.class.getClassLoader().getResource(libName);
    String libFile = libLocation.getPath();
    System.load(libFile);
  }

  static native long compressInMemory(int width, int height, int[] imageData, float compression);
  static native int readSomeCompressed(long ptr, byte[] buffer, long pos);
  static native void freeCompressed(long ptr);

  static void write(OutputStream out, BitmapImage image) throws IOException {
    byte[] buffer = new byte[1024*1024];
    long ptr = compressInMemory(image.width, image.height, image.data, 1.0f);
    long pos = 0;
    try {
      while(true) {
        int read = readSomeCompressed(ptr, buffer, pos);
        if(read == 0)
          break;
        out.write(buffer, 0, read);
        pos += read;
      }
    }
    finally {
      freeCompressed(ptr);
    }
  }
}
