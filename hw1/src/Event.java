/**
 * This class represents an event on the M/M/1 queuing system.
 */
public final class Event {

  private final long executionTime;

  private final long arrivalTime;

  private long departureTime;

  private boolean served;

  public Event(final long executionTime,
               final long arrivalTime) {
    this.executionTime = executionTime;
    this.arrivalTime = arrivalTime;
    this.served = false;
  }

  public long getExecutionTime() {
    return executionTime;
  }

  public long getDelayTime(final long currentTime) {
    if (served) {
      return departureTime - arrivalTime;
    } else {
      return currentTime - arrivalTime;
    }
  }

  public void start(final long currentTime) {
    this.departureTime = currentTime;
    this.served = true;
  }
}
