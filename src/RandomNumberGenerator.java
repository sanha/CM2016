/**
 * Utility class generates random number.
 */
public class RandomNumberGenerator {

  /**
   * @return random number from uniform(0, 1)
   */
  private static double ranf() {
    return Math.random();
  }

  /**
   * @param mean the mean of this exponential distribution
   * @return a psuedo-random exponential distribution value
   */
  public static long expntl(final double mean) {
    return (long)-(mean * Math.log(ranf()));
  }
}
