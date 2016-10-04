import java.util.LinkedList;
import java.util.Queue;

/**
 * This class represents a queue contains events.
 */
public final class EventQueue {

  private Queue<Event> queue;

  private double timedQueueLength;

  private long lastTimestamp;

  public EventQueue() {
    this.queue = new LinkedList<>();
    this.timedQueueLength = 0;
    this.lastTimestamp = Long.MIN_VALUE;
  }

  public void addEvent(final Event event, final long currentTimestamp) {
    if (lastTimestamp == Long.MIN_VALUE) {
      lastTimestamp = currentTimestamp;
    } else {
      timedQueueLength += queue.size() * (currentTimestamp - lastTimestamp);
      lastTimestamp = currentTimestamp;
    }
    queue.add(event);
  }

  public Event pollEvent(final long currentTimestamp) throws RuntimeException {
    if (queue.isEmpty()) {
      throw new RuntimeException("QUEUE: System polled empty event queue.");
    } else {
      timedQueueLength += queue.size() * (currentTimestamp - lastTimestamp);
      lastTimestamp = currentTimestamp;
      return queue.poll();
    }
  }

  public Queue<Event> getQueue() {
    return queue;
  }

  public double getTimedQueueLength() {
    return timedQueueLength;
  }

  public boolean isEmpty() {
    return queue.isEmpty();
  }
}
