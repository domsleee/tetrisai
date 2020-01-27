import axios from 'axios';

const URL = 'http://localhost:5000/get-file';

export class GetOldCapture {
  public async getJustBeforeTransition() {
    const file = 'new_transition.txt';
    return await this.req(file);
  }

  public async getGreenFrozen() {
    const file = 'green_freeze.txt';
    return await this.req(file);
  }

  public async getProblematicPlacement() {
    const file = 'problematic_placement.txt';
    return await this.req(file);
  }

  private async req(file: string) {
    const res: { data: { result: any }; status: number } = await axios.post(
      URL,
      {
        file
      }
    );
    console.log(res);
    return res.data.result;
  }
}
