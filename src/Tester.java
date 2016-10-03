import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class tests MM1 queuing system.
 */
public final class Tester {

  private static final Logger logger = Logger.getLogger(Tester.class.getName());

  public static void main(final String[] args) throws Exception {
    addFileHandler(logger);

    final MM1System system = new MM1System(10000, 0.95, 50, logger);

    return ;
  }

  private static void addFileHandler(Logger logger) {
    try {
      final FileHandler fileHandler = new FileHandler(Tester.class.getName() + ".log");
      logger.addHandler(fileHandler);
    } catch (Exception e) {
      logger.log(Level.SEVERE, null, e);
    }
  }
}
