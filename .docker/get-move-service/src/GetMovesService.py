from interfaces.IGetMoves import IGetMoves
from GetMoves import GetMoves
import asyncio

loop = asyncio.get_event_loop()
get_moves_service: IGetMoves = GetMoves(loop=loop)
