/**
 * This class represents an event on the M/M/1 queuing system.
 */
public final class Event {

  private final long executionTime;

  private final long arrivalTime;

  private long departureTime;

  public Event(final long executionTime,
               final long arrivalTime) {
    this.executionTime = executionTime;
    this.arrivalTime = arrivalTime;
  }

  public long getExecutionTime() {
    return executionTime;
  }

  public long getDelayTime() {
    return departureTime - arrivalTime;
  }

  public void start() {
    this.departureTime = System.currentTimeMillis();
  }
}
