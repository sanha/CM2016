import java.util.concurrent.ScheduledExecutorService;
import java.util.logging.Logger;

/**
 * This class represents a M/M/1 queuing system.
 */
public final class MM1System {
  private final Logger LOG;

  private final double timeLimit;

  private long startTime;

  private long duration;

  private final EventQueue eventQueue;

  private final EventGenerator eventGenerator;

  private final Server server;

  private int totalServedJob;

  private double totalDelayTime;

  private boolean end;

  public MM1System(final double timeLimit,
                   final double trafficLoad,
                   final long muOverOne,
                   final Logger logger,
                   final ScheduledExecutorService scheduler) {
    this.timeLimit = timeLimit;
    this.eventQueue = new EventQueue();
    this.eventGenerator = new EventGenerator(muOverOne, trafficLoad, scheduler, eventQueue);
    this.server = new Server();
    this.LOG = logger;
    this.duration = Long.MIN_VALUE;
    this.totalServedJob = 0;
    this.totalDelayTime = 0;
    this.end = false;
  }

  public void start() throws InterruptedException, RuntimeException {
    this.startTime = System.currentTimeMillis();
    eventGenerator.start();
    LOG.info("MM1 System starts at " + startTime);
    // System.out.println("MM1 System starts at " + startTime);

    while(System.currentTimeMillis() - startTime < timeLimit) {
      if(eventQueue.isEmpty()) {
        System.out.println("Empty");
        continue;
      }
      final long delay = server.process(eventQueue.pollEvent());
      if (delay == Long.MIN_VALUE) {
        throw new RuntimeException("SYSTEM: Server process method is called during non-idle state.");
      } else {
        totalServedJob ++;
        totalDelayTime += delay;
      }
    }
    stop();
  }

  private void stop() {
    this.duration = System.currentTimeMillis() - startTime;
    eventGenerator.close();
    this.end = true;
    LOG.info("MM1 System ends at " + (duration + startTime) +", duration is " + duration);
    LOG.info("Average job number: " + getAvgJobs());
    LOG.info("Average queue length: " + getAvgQueueLength());
    LOG.info("Average delay per a job:" + getAvgDelay());
    LOG.info("Total served job: " + totalServedJob);
    // System.out.println("MM1 System ends at " + (duration + startTime) +", duration is " + duration);
    // System.out.println(("Average job number: " + getAvgJobs()));
    // System.out.println("Average queue length: " + getAvgQueueLength());
    // System.out.println("Average delay per a job:" + getAvgDelay());
  }

  public double getAvgQueueLength() {
    if (end) {
      return eventQueue.getTimedQueueLength() / duration;
    } else {
      throw new RuntimeException("SYSTEM: System is not ended yet.");
    }
  }

  public double getAvgJobs() {
    if (end) {
      return getAvgQueueLength() + (server.getBusyTime() / duration);
    } else {
      throw new RuntimeException("SYSTEM: System is not ended yet.");
    }

  }

  public double getAvgDelay() {
    if (end) {
      return totalDelayTime / totalServedJob;
    } else {
      throw new RuntimeException("SYSTEM: System is not ended yet.");
    }
  }
}
