import asyncio
import subprocess

class SimpleProcess:
  def __init__(self, args: list):
    self._args = args
    self._process = None
    self._timeout = 1.0
  
  async def init(self):
    self._process = await asyncio.create_subprocess_exec(*self._args,
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  
  def read_line(self):
    async def fn():
      return await asyncio.wait_for(self._process.stdout.readline(), self._timeout)
    line = asyncio.get_event_loop().run_until_complete(fn())
    return line.decode('utf-8').strip('\n')
  
  def is_empty(self):
    # StreamReader does not offer peek().
    return True
  
  def send_line(self, b: bytes):
    self._process.stdin.write(b)
  
  def close(self):
    async def fn():
      await self._process.wait()
    asyncio.get_event_loop().run_until_complete(fn())

