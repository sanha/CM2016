import java.io.FileInputStream;
import java.io.FileOutputStream;

public final class SplitResult {
  final static int scale = 100;

  public static void main(final String[] args) throws Exception {
    splitResult();
  }

  private static void splitResult() {
    final String filePath = "ideal/L/";
    final String targetName = "AnalL";
    final boolean LOrW = true;
    final FileInputStream fileStream;
    final FileOutputStream outputStream1;
    final FileOutputStream outputStream2;
    final FileOutputStream outputStream3;
    final FileOutputStream outputStream4;
    FileOutputStream outputStream5 = null;

    try
    {
      fileStream = new FileInputStream(filePath + "ideal" + targetName + ".txt");
      outputStream1 = new FileOutputStream(filePath + targetName + "1.txt");
      outputStream2 = new FileOutputStream(filePath + targetName + "2.txt");
      outputStream3 = new FileOutputStream(filePath + targetName + "3.txt");
      outputStream4 = new FileOutputStream(filePath + targetName + "4.txt");
      if (LOrW) {
        outputStream5 = new FileOutputStream(filePath + targetName + ".txt");
      }

      byte[ ] readBuffer = new byte[fileStream.available()];
      while (fileStream.read(readBuffer) != -1);

      final String result = new String(readBuffer);
      final String[] array = result.split("\n");
      for(int i = 0; i < array.length; i++) {
        final String[] tmpStr = array[i].trim().replaceAll(" + ", " ").split(" ");
        outputStream1.write((tmpStr[0] + " " + tmpStr[1] + "\n").getBytes());
        outputStream2.write((tmpStr[0] + " " + tmpStr[2] + "\n").getBytes());
        outputStream3.write((tmpStr[0] + " " + tmpStr[3] + "\n").getBytes());
        outputStream4.write((tmpStr[0] + " " + tmpStr[4] + "\n").getBytes());
        if (LOrW) {
          outputStream5.write((tmpStr[0] + " " + tmpStr[5] + "\n").getBytes());
        }
      }
      outputStream1.close();
      outputStream2.close();
      outputStream3.close();
      outputStream4.close();
      if (LOrW) {
        outputStream1.close();
      }
      fileStream.close();
    }
    catch (final Exception e) {
      System.err.println(e);
    }
  }

  private static void makeCmd(final int clusterCount) {
    String filePath = "plot/plot.cmd";
    final FileOutputStream outputStream;

    try
    {
      String result = "set nokey\n" +
          "set xlab \"x\" \n" +
          "set ylab \"y\" \n" +
          "set grid\n" +
          "set xrange[-" + scale + ".0:" + scale + ".0]\n" +
          "set yrange[-" + scale + ".0:" + scale + ".0]\n" +
          "set xtics -" + scale + ".0, 10.0, " + scale + ".0\n" +
          "set ytics -" + scale + ".0, 10.0, " + scale + ".0\n" +
          "plot \"cluster1.txt\"\n";
      for (int i = 2; i <= clusterCount; i++) {
        result += "replot \"cluster" + i + ".txt\"\n";
      }

      outputStream = new FileOutputStream(filePath);
      outputStream.write(result.getBytes());
      outputStream.close();
    }
    catch (final Exception e) {
      System.err.println(e);
    }
  }
}
