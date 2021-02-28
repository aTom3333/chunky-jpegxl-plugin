package dev.ferrand.chunky.jpegxl;

import se.llbit.chunky.Plugin;
import se.llbit.chunky.main.Chunky;
import se.llbit.chunky.main.ChunkyOptions;
import se.llbit.chunky.renderer.export.PictureExportFormats;
import se.llbit.chunky.ui.ChunkyFx;

public class JpegxlPlugin implements Plugin {
  @Override
  public void attach(Chunky chunky) {
    PictureExportFormats.registerFormat(new JpegxlExportFormat());
  }

  public static void main(String[] args) {
    // Start Chunky normally with this plugin attached.
    Chunky.loadDefaultTextures();
    Chunky chunky = new Chunky(ChunkyOptions.getDefaults());
    new JpegxlPlugin().attach(chunky);
    ChunkyFx.startChunkyUI(chunky);
  }
}
