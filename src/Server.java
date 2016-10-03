/**
 * This class represents a server that process received events.
 */
public class Server {

  private boolean isIdle;

  private double busyTime;

  public Server() {
    this.isIdle = true;
    this.busyTime = 0;
  }

  public long process(final Event event) throws InterruptedException {
    if (isIdle) {
      isIdle = false;
      busyTime += event.getExecutionTime();
      event.start(System.currentTimeMillis());
      Thread.sleep(event.getExecutionTime());
      isIdle = true;
      return event.getDelayTime();
    } else {
      // throw new RuntimeException("SERVER: Process method is called during non-idle state.");
      return Long.MIN_VALUE;
    }
  }

  public boolean isIdle() {
    return isIdle;
  }

  public double getBusyTime() {
    return busyTime;
  }
}
