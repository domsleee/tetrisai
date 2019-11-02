from tetrisai import runner
from tetrisai.runner_settings import RunnerSettings
import argparse
import logging

logging.basicConfig()

def main(args):
  runner_settings = RunnerSettings()
  print(runner_settings)
  runner.MyRunner(args.binary, runner_settings).run()


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("binary", help="binary used for PSO")
  args = parser.parse_args()
  main(args)

