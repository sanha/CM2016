/**
 * This class represents a server that process received events.
 */
public class Server {

  private double busyTime;

  public Server() {
    this.busyTime = 0;
  }

  public long process(final Event event) {
    busyTime += event.getExecutionTime();
    return event.getExecutionTime();
  }

  public double getBusyTime() {
    return busyTime;
  }
}
