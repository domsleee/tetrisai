import asyncio
import subprocess

class SimpleProcess:
  def __init__(self, args: list, loop: any=asyncio.get_event_loop()):
    self._args = args
    self._process = None
    self._timeout = 30.0
    self._loop = loop
  
  async def init(self):
    self._process = await asyncio.create_subprocess_exec(*self._args,
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  
  def read_line(self):
    async def fn():
      try:
        await self._process.stdin.drain()
      except Exception as e:
        print(self._process.returncode)
        raise e
      return await asyncio.wait_for(self._process.stdout.readline(), self._timeout)
    line = self._loop.run_until_complete(fn())
    return line.decode('utf-8').strip('\n')
  
  def is_empty(self):
    # StreamReader does not offer peek().
    return True
  
  def send_line(self, b: bytes):
    self._process.stdin.write(b)
  
  def close(self):
    async def fn():
      await self._process.wait()
    self._loop.run_until_complete(fn())

