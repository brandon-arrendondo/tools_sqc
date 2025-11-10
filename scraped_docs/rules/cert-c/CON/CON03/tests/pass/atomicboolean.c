final class ControlledStop implements Runnable {
  private final AtomicBoolean done = new AtomicBoolean(false);
 
  @Override public void run() {
    while (!done.get()) {
      try {
        // ...
        Thread.currentThread().sleep(1000); // Do something
      } catch(InterruptedException ie) { 
        Thread.currentThread().interrupt(); // Reset interrupted status
      } 
    } 	 
  }

  public void shutdown() {
    done.set(true);
  }
}