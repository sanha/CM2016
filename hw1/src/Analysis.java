import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class analyses MM1 queuing system.
 */
public final class Analysis {

  private static final Logger logger = Logger.getLogger(Analysis.class.getName());

  public static void main(final String[] args) throws Exception {
    addFileHandler(logger);

    final long defaultMuOverOne = 100;

    String result = "TrafficLoad AvgJobs AvgQLength AvgDelay \n";

    for (double trafficLoad = 0.0; trafficLoad < 0.96; trafficLoad += 0.025) {
      double avgJobs = trafficLoad / (1 - trafficLoad);
      double avgQLength = avgJobs * trafficLoad;
      double avgDelay = avgJobs * defaultMuOverOne;

      result += (trafficLoad + " " + avgJobs + " " + avgQLength + " " + avgDelay + "\n");
    }

    logger.info(result);
    return ;
  }

  private static void addFileHandler(Logger logger) {
    try {
      final FileHandler fileHandler = new FileHandler(Analysis.class.getName() + ".log");
      logger.addHandler(fileHandler);
    } catch (Exception e) {
      logger.log(Level.SEVERE, null, e);
    }
  }
}
