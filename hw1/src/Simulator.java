import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class simulates MM1 queuing system.
 */
public final class Simulator {

  private static final Logger logger = Logger.getLogger(Simulator.class.getName());

  public static void main(final String[] args) throws Exception {
    addFileHandler(logger);

    final long stepLimit = 10000000;

    final long defaultMuOverOne = 100;

    final long iteratePerTraffic = 10;

    String result = "TrafficLoad AvgJobs AvgQLength AvgDelay \n";

    for (double trafficLoad = 0.0; trafficLoad < 0.96; trafficLoad += 0.025) {
      double avgJobs = 0;
      double avgQLength = 0;
      double avgDelay = 0;
      for (int i=0; i < iteratePerTraffic; i++) {
        final MM1System system = new MM1System(stepLimit, trafficLoad, defaultMuOverOne, logger);
        avgJobs += system.getAvgJobs();
        avgQLength += system.getAvgQueueLength();
        avgDelay += system.getAvgDelay();
      }

      avgJobs /= iteratePerTraffic;
      avgQLength /= iteratePerTraffic;
      avgDelay /= iteratePerTraffic;

      //result += (trafficLoad + " " + avgJobs + " " + avgQLength + " " + avgDelay + "\n");
      result += (trafficLoad + " " + avgJobs + " " + avgQLength + " " + avgDelay + "\n");
      //result += (trafficLoad + " " + avgJobs + " " + avgQLength + " " + avgDelay + "\n");
      //result += (trafficLoad + " " + avgJobs + " " + avgQLength + " " + avgDelay + "\n");
    }

    /*
    final MM1System system = new MM1System(stepLimit, 1.0, defaultMuOverOne, logger);
    result += (1.0 + " " + system.getAvgJobs() + " " + system.getAvgQueueLength() + " " + system.getAvgDelay() + "\n");
    final MM1System system2 = new MM1System(stepLimit, 1.5, defaultMuOverOne, logger);
    result += (1.5 + " " + system2.getAvgJobs() + " " + system2.getAvgQueueLength() + " " + system2.getAvgDelay() + "\n");
    */

    logger.info(result);
    return ;
  }

  private static void addFileHandler(Logger logger) {
    try {
      final FileHandler fileHandler = new FileHandler(Simulator.class.getName() + ".log");
      logger.addHandler(fileHandler);
    } catch (Exception e) {
      logger.log(Level.SEVERE, null, e);
    }
  }
}
