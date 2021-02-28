package dev.ferrand.chunky.jpegxl;

import se.llbit.chunky.renderer.export.PictureExportFormat;
import se.llbit.chunky.renderer.scene.Scene;
import se.llbit.chunky.resources.BitmapImage;
import se.llbit.util.TaskTracker;

import java.io.IOException;
import java.io.OutputStream;

public class JpegxlExportFormat implements PictureExportFormat {
  @Override
  public String getName() {
    return "JPEG-XL";
  }

  @Override
  public String getExtension() {
    return ".jxl";
  }

  @Override
  public void write(OutputStream outputStream, Scene scene, TaskTracker taskTracker) throws IOException {
    BitmapImage image = scene.getBackBuffer();
    JpegxlWriter.write(outputStream, image);
  }

  @Override
  public String getDescription() {
    return "New (as of 2021) image format that offers a great compression for great visual quality";
  }

  @Override
  public boolean isTransparencySupported() {
    return true;
  }
}
