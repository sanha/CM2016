import java.util.Collection;
import java.util.LinkedList;
import java.util.function.BiConsumer;

/**
 * This class simulates MM1 queuing system.
 */
public final class Tmp {

  public static void main(final String[] args) throws Exception {

    final Collection<String> collection = new LinkedList<>();
    BiConsumer<Object, Collection<String>> cons = (input, state) -> state.add(input.toString());
    cons.accept(10, collection);

    System.out.println(collection);

    Integer i = 0;
    BiConsumer<Integer, Integer> cons2 = (input, state) -> state = input;
    cons2.accept(100, i);

    System.out.println(i);

    final A a = new A(0);
    BiConsumer<Integer, A> cons3 = (input, state) -> state.setI(input);
    cons3.accept(100, a);

    System.out.println(a.getI());

    return ;
  }

  private static final class A {
    private int i;

    public A(int i) {
      this.i = i;
    }

    public int getI() {
      return i;
    }

    public void setI(int i) {
      this.i = i;
    }
  }
}
