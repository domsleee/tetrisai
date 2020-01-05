
import asyncio
import threading



class IOReader:
  def __init__(self, stream, timeout: float):
    self._stream = stream
    self._timeout = timeout

  def read_line(self):
    async def fn():
      return await asyncio.wait_for(self._stream.readline(), self._timeout)
    print("event loop", asyncio.get_event_loop())
    line = asyncio.get_event_loop().run_until_complete(fn())
    return line.decode('utf-8').strip('\n')

  def is_empty(self) -> bool:
    return True
    async def peek():
      return self._stream.peek() == None
    async def check_peek():
      return await asyncio.wait_for(peek(), timeout=0.1)

    return asyncio.run(check_peek())

