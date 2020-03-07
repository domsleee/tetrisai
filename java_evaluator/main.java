package java_evaluator;
import java.util.*;

class Main {
  public static void main(String[] args) throws Exception {
    Scanner scanner = new Scanner(System.in);

    boolean [][] playfield = new boolean[Evaluator.HEIGHT][Evaluator.WIDTH];

    for (int i = 0; i < Evaluator.HEIGHT; ++i) {
      String s = scanner.nextLine();
      if (s.length() != Evaluator.WIDTH) {
        throw new Exception("no!");
      }
      for (int j = 0; j < Evaluator.WIDTH; ++j) {
        playfield[i][j] = (s.charAt(j) == '1');
      }
    }

    double [] weights = new double[Evaluator.NUM_WEIGHTS];
    for (int i = 0; i < Evaluator.NUM_WEIGHTS; ++i) {
      if (i > 0) weights[i-1] = 0;
      weights[i] = 1;
      Evaluator e = new Evaluator();
      double score = e.evaluate(0, 0, playfield, weights);
      System.out.printf("%.8f\n", score);
    }
  }
}
