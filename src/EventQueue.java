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

  public void addEvent(final Event event) {
    if (lastTimestamp == Long.MIN_VALUE) {
      lastTimestamp = System.currentTimeMillis();
    } else {
      final long currentTimestamp = System.currentTimeMillis();
      timedQueueLength += queue.size() * (currentTimestamp - lastTimestamp);
      lastTimestamp = currentTimestamp;
    }
    queue.add(event);
  }

  public Event pollEvent() throws RuntimeException {
    if (queue.isEmpty()) {
      throw new RuntimeException("QUEUE: System polled empty event queue.");
    } else {
      final long currentTimestamp = System.currentTimeMillis();
      timedQueueLength += queue.size() * (currentTimestamp - lastTimestamp);
      lastTimestamp = currentTimestamp;
      return queue.poll();
    }
  }

  public double getTimedQueueLength() {
    return timedQueueLength;
  }

  public boolean isEmpty() {
    return queue.isEmpty();
  }
}
