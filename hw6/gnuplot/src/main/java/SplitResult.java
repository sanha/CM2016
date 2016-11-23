import java.io.FileInputStream;
import java.io.FileOutputStream;

public final class SplitResult {
  final static int scale = 100;

  public static void main(final String[] args) throws Exception {

    //splitOmnetResult("background", "small", 10, 0.6);
    //splitOmnetResult("polling", "small", 10, 0.6);
    //splitOmnetResult("deferrable", "small", 10, 0.6);
    //splitOmnetResult("sporadic", "small", 10, 0.6);

    //splitOmnetResult("background", "big", 1000, 60);
    //splitOmnetResult("polling", "big", 1000, 60);
    //splitOmnetResult("deferrable", "big", 1000, 60);
    //splitOmnetResult("sporadic", "big", 1000, 60);

    reverseFile("small/background");
    reverseFile("small/polling");
    reverseFile("small/deferrable");
    reverseFile("small/sporadic");
    reverseFile("big/background");
    reverseFile("big/polling");
    reverseFile("big/deferrable");
    reverseFile("big/sporadic");
  }

  private static void reverseFile(final String path) {
    final FileInputStream fileStream;
    final FileOutputStream outputStream;

    try
    {
      fileStream = new FileInputStream(path + ".txt");
      outputStream = new FileOutputStream(path + "_r.txt");

      byte[ ] readBuffer = new byte[fileStream.available()];
      while (fileStream.read(readBuffer) != -1);

      final String result = new String(readBuffer);
      final String[] array = result.split("\n");
      for (int i = 0; i < array.length; i++) {
        outputStream.write((array[array.length - i - 1] + "\n").getBytes());
      }
      outputStream.close();
      fileStream.close();
    }
    catch (final Exception e) {
      System.err.println(e);
    }
  }

  private static void splitOmnetResult(final String server, final String target, final double max, final double deduc) {
    final FileInputStream fileStream;
    final FileOutputStream outputStream;

    try
    {
      fileStream = new FileInputStream(server + "/" + target + ".txt");
      outputStream = new FileOutputStream(target + "/" + server +".txt");

      byte[ ] readBuffer = new byte[fileStream.available()];
      while (fileStream.read(readBuffer) != -1);

      final String result = new String(readBuffer);
      final String[] array = result.split("\n");
      for (int i = 0; i < array.length; i++) {
        String lineResult = (max - deduc * i) + " ";

        final String[] tmpStr = array[i].split(":");
        final String tmpResult = tmpStr[2];
        lineResult += tmpResult + "\n";

        outputStream.write(lineResult.getBytes());
      }
      outputStream.close();
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
