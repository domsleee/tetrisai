import axios from 'axios';

const URL = 'http://localhost:5000/get-file';

export class GetOldCapture {
  public async getJustBeforeTransition() {
    const file = 'new_transition.txt';
    const res: { data: {result: any}, status: number } = await axios.post(URL, {
      file,
    });
    console.log(res);
    return res.data.result;
  }
}
