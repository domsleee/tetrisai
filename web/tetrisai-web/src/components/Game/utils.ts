
export function arraySubtract(arr1: any[], arr2: any[]) {
  let res: any[] = [];
  let s2 = new Set();
  for (const el of arr2) s2.add(el);
  for (const el of arr1) {
    if (!s2.has(el)) res.push(el);
  }
  return res;
}