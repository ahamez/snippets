fn foo(x: &mut [u32])
{
  println!("{:?}", x);
  x[0] = 100;
}

fn main()
{
  let mut v0 = vec![0, 1, 2, 3];
  println!("{:?}", v0);

  let a0 = [10, 11, 12, 13];
  println!("{:?}", a0);

  {
    let s0_v0 : &mut [u32] = &mut v0;
    println!("{:?}", s0_v0);
  }

  {
    let s0_v0 = &mut v0;
    println!("{:?}", s0_v0);
  }

  {
    let s1_v0 = &mut v0[0..2];  
    println!("{:?}", s1_v0);
  }

  {
    foo(&mut v0);
    println!("{:?}", v0);
  }

  {
    for i in &mut v0
    {
      *i = 42;
    }
    println!("{:?}", v0);
  }

  {
    for i in &mut v0[1..3]
    {
      *i = 43;
    }
    println!("{:?}", v0);
  }
  {
    let v = vec![0,1,2];
    println!("{:?}", &v[..]);
  }
}
