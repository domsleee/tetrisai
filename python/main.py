from tetrisai import runner
from tetrisai.runner_settings import RunnerSettings
import argparse
import logging
import os

logging.basicConfig()

DIR = os.path.dirname(os.path.realpath(__file__))
DEFAULT_BINARY = os.path.join(DIR, '..', 'bazel-bin', 'src', 'ew')

def main(args):
  runner_settings = RunnerSettings(num_particles=2)
  print(runner_settings)
  runner.MyRunner(args.binary, runner_settings).run()


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--binary", help="binary used for PSO", default=DEFAULT_BINARY)
  args = parser.parse_args()
  main(args)

