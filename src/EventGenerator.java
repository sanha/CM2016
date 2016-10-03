import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * This class represents an event generator.
 */
public final class EventGenerator {

  private final AtomicBoolean started;

  private final long muOverOne;

  private final double lambdaOverOne;

  private final ScheduledExecutorService scheduler;

  private ScheduledFuture result;

  private EventQueue queue;

  public EventGenerator(final long muOverOne,
                        final double trafficLoad,
                        final ScheduledExecutorService scheduler,
                        final EventQueue eventQueue) {
    this.muOverOne = muOverOne;
    this.lambdaOverOne = muOverOne / trafficLoad;
    this.scheduler = scheduler;
    this.started = new AtomicBoolean(false);
    this.queue = eventQueue;
  }

  private Event getEvent() {
    return new Event(RandomNumberGenerator.expntl(muOverOne), System.currentTimeMillis());
  }

  public void start() {
    if (started.compareAndSet(false, true)) {
      result = scheduler.scheduleAtFixedRate(new Runnable() {
        public void run() {
          queue.addEvent(getEvent());
        }
      }, RandomNumberGenerator.expntl(lambdaOverOne), RandomNumberGenerator.expntl(lambdaOverOne), TimeUnit.MILLISECONDS);
    }
  }

  public void close() {
    result.cancel(true);
  }
}
