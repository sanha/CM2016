import java.util.Queue;
import java.util.logging.Logger;

/**
 * This class represents a M/M/1 queuing system.
 */
public final class MM1System {
  private final Logger LOG;

  private final double timeLimit;

  private long currentTime;

  private long duration;

  private final EventQueue eventQueue;

  private final EventGenerator eventGenerator;

  private final Server server;

  private int totalServedEvents;

  private double servedDelayTime;

  private boolean end;

  private long eventCreateTime;

  private long serverIdleTime;

  public MM1System(final double timeLimit,
                   final double trafficLoad,
                   final long muOverOne,
                   final Logger logger) throws RuntimeException {
    this.timeLimit = timeLimit;
    this.eventQueue = new EventQueue();
    this.eventGenerator = new EventGenerator(muOverOne, trafficLoad);
    this.server = new Server();
    this.LOG = logger;
    this.duration = Long.MIN_VALUE;
    this.totalServedEvents = 0;
    this.servedDelayTime = 0;
    this.end = false;
    this.currentTime = 0;
    eventCreateTime = eventGenerator.getEventInterval();
    this.serverIdleTime = Long.MIN_VALUE;
    LOG.info("MM1 System starts");
    operate();
  }

  private void operate() {
    while(currentTime < timeLimit) {
      if (currentTime >= eventCreateTime) {
        eventQueue.addEvent(eventGenerator.getEvent(currentTime), currentTime);
        eventCreateTime = currentTime + eventGenerator.getEventInterval();
      }
      if(eventQueue.isEmpty()) {
        currentTime++;
        continue;
      }
      if (currentTime >= serverIdleTime) {
        final Event event = eventQueue.pollEvent(currentTime);
        event.start(currentTime);
        serverIdleTime = currentTime + server.process(event);

        totalServedEvents++;
        servedDelayTime += event.getDelayTime(currentTime);
      }
      currentTime++;
    }
    stop();
  }

  private void stop() {
    this.duration = currentTime;
    this.end = true;

    LOG.info("MM1 System ends at " + duration);
    /*
    LOG.info("Average job number: " + getAvgJobs());
    LOG.info("Average queue length: " + getAvgQueueLength());
    LOG.info("Average delay per a job:" + getAvgDelay());
     */
    LOG.info("Total served job: " + totalServedEvents);
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
      final Queue<Event> queue = eventQueue.getQueue();
      long notServedDelayTime = 0;
      final long notServedEvents = queue.size();
      while(!queue.isEmpty()) {
        final Event event = queue.poll();
        notServedDelayTime += event.getDelayTime(currentTime);
      }

      return (servedDelayTime + notServedDelayTime) / (totalServedEvents + notServedEvents);
    } else {
      throw new RuntimeException("SYSTEM: System is not ended yet.");
    }
  }
}
