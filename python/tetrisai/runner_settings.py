import dataclasses
import typing

LOCAL = False
NUM_PARTICLES = 100
NUM_NEIGHBOURS = 100
NUM_ITERATIONS = 6900
VELOCITY_CLAMP = (-5, 5)
DIMENSIONS = 17

@dataclasses.dataclass
class RunnerSettings:
  num_particles: int = NUM_PARTICLES
  num_neighbours: int = NUM_NEIGHBOURS
  iterations: int = NUM_ITERATIONS
  local: bool = LOCAL
  velocity_clamp: typing.Tuple[float, float] = VELOCITY_CLAMP
  optimizer_options: dict = dataclasses.field(default_factory=dict)

  def get_optimizer_options(self):
    if self.local:
      options = {'c1': 2, 'c2': 2, 'w': 0.728}
      options['k'] = self.num_neighbours
      options['p'] = 2
      self.optimizer_options = options
    else:
      self.optimizer_options = {'c1': 2.5, 'c2': 2.5, 'w': 0.92}

    return {
      'n_particles': self.num_particles,
      'dimensions': DIMENSIONS,
      'velocity_clamp': self.velocity_clamp,
      'options': self.optimizer_options
    }
