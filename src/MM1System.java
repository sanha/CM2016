import java.util.logging.Logger;

/**
 * This class represents a M/M/1 queuing system.
 */
public final class MM1System {
  private final Logger LOG;

  private final double timeLimit;

  private final long startTime;

  private long duration;

  private final EventQueue eventQueue;

  private final EventGenerator eventGenerator;

  private final Server server;

  private int totalServedJob;

  private double totalDelayTime;

  private boolean end;

  private long eventCreateTime;

  private long serverIdleTime;

  public MM1System(final double timeLimit,
                   final double trafficLoad,
                   final long muOverOne,
                   final Logger logger) throws InterruptedException, RuntimeException {
    this.timeLimit = timeLimit;
    this.eventQueue = new EventQueue();
    this.eventGenerator = new EventGenerator(muOverOne, trafficLoad);
    this.server = new Server();
    this.LOG = logger;
    this.duration = Long.MIN_VALUE;
    this.totalServedJob = 0;
    this.totalDelayTime = 0;
    this.end = false;
    this.startTime = System.currentTimeMillis();
    eventCreateTime = startTime + eventGenerator.getEventInterval();
    this.serverIdleTime = Long.MIN_VALUE;
    LOG.info("MM1 System starts at " + startTime);
    operate();
  }

  private void operate() {
    long currentTime = System.currentTimeMillis();
    while(currentTime - startTime < timeLimit) {
      if (currentTime >= eventCreateTime) {
        eventQueue.addEvent(eventGenerator.getEvent());
        eventCreateTime = currentTime + eventGenerator.getEventInterval();
      }
      if(eventQueue.isEmpty()) {
        currentTime = System.currentTimeMillis();
        continue;
      }
      if (currentTime >= serverIdleTime) {
        final Event event = eventQueue.pollEvent();
        event.start();
        serverIdleTime = currentTime + server.process(event);

        totalServedJob++;
        totalDelayTime += event.getDelayTime();
      }
      currentTime = System.currentTimeMillis();
    }
    stop();
  }

  private void stop() {
    this.duration = System.currentTimeMillis() - startTime;
    this.end = true;
    LOG.info("MM1 System ends at " + (duration + startTime) +", duration is " + duration);
    LOG.info("Average job number: " + getAvgJobs());
    LOG.info("Average queue length: " + getAvgQueueLength());
    LOG.info("Average delay per a job:" + getAvgDelay());
    LOG.info("Total served job: " + totalServedJob);
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
