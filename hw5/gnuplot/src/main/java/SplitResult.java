import java.io.FileInputStream;
import java.io.FileOutputStream;

public final class SplitResult {
  final static int scale = 100;

  public static void main(final String[] args) throws Exception {

    splitOmnetResult("default/", "defaultSim");

    splitSmplResult("default", "default/L/", "SimL", true);
    splitSmplResult("default", "default/W/", "SimW", true);
    splitSmplResult("default", "default/U/", "SimU", false);
  }

  private static void splitOmnetResult(final String filePath, final String target) {
    final FileInputStream fileStream;
    final FileOutputStream lOutputStream;
    final FileOutputStream wOutputStream;
    final FileOutputStream uOutputStream;

    try
    {
      fileStream = new FileInputStream(filePath + "result.txt");
      lOutputStream = new FileOutputStream(filePath + "L/" + target + "L.txt");
      wOutputStream = new FileOutputStream(filePath + "W/" + target + "W.txt");
      uOutputStream = new FileOutputStream(filePath + "U/" + target + "U.txt");

      byte[ ] readBuffer = new byte[fileStream.available()];
      while (fileStream.read(readBuffer) != -1);

      final String result = new String(readBuffer);
      final String[] array = result.split("\n");
      for (int i = 0; i < array.length / 4; i++) {
        String lResult = ((Double)(0.05 * (i + 1))).toString() + " ";
        String wResult = ((Double)(0.05 * (i + 1))).toString() + " ";
        String uResult = ((Double)(0.05 * (i + 1))).toString() + " ";
        double lAccum = 0;
        double wAccum = 0;

        for (int j = 0; j < 4; j++) {
          final String[] tmpStr = array[4*i + j].split(":");
          final String[] tmpResult = tmpStr[2].split(" ");
          lResult += tmpResult[0] + " ";
          wResult += tmpResult[1] + " ";
          uResult += tmpResult[2] + " ";
          lAccum += Double.parseDouble(tmpResult[0]);
          wAccum += Double.parseDouble(tmpResult[1]);
        }
        lResult += lAccum + "\n";
        wResult += wAccum + "\n";
        uResult += "\n";

        lOutputStream.write(lResult.getBytes());
        wOutputStream.write(wResult.getBytes());
        uOutputStream.write(uResult.getBytes());
      }
      lOutputStream.close();
      wOutputStream.close();
      uOutputStream.close();
      fileStream.close();
    }
    catch (final Exception e) {
      System.err.println(e);
    }
  }

  private static void splitSmplResult(
      final String setting, final String filePath, final String targetName, final boolean LOrW) {
    final FileInputStream fileStream;
    final FileOutputStream outputStream1;
    final FileOutputStream outputStream2;
    final FileOutputStream outputStream3;
    final FileOutputStream outputStream4;
    FileOutputStream outputStream5 = null;

    try
    {
      fileStream = new FileInputStream(filePath + setting + targetName + ".txt");
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
