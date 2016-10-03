import java.util.concurrent.atomic.AtomicBoolean;

/**
 * This class represents an event generator.
 */
public final class EventGenerator {

  private final AtomicBoolean started;

  private final long muOverOne;

  private final double lambdaOverOne;

  public EventGenerator(final long muOverOne,
                        final double trafficLoad) {
    this.muOverOne = muOverOne;
    this.lambdaOverOne = muOverOne / trafficLoad;
    this.started = new AtomicBoolean(false);
  }

  public Event getEvent() {
    return new Event(RandomNumberGenerator.expntl(muOverOne), System.currentTimeMillis());
  }

  public long getEventInterval() {
    return RandomNumberGenerator.expntl(lambdaOverOne);
  }
}
