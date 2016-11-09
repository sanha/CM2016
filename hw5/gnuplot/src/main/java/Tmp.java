public final class Tmp {
  final static int scale = 100;

  public static void main(final String[] args) throws Exception {

    for (int i=1; i<=20; i++) {
      System.out.println("//fifonet.FIFONet.gen.sendIaTime = exponential(" + 1 / (0.05 * i) + "s)");
    }
  }
}
