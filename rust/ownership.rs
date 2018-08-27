use std::rc::Rc;

struct Bar {
  name: String,
}

#[derive(Copy, Clone)]
struct BazCopyable {
  x: u32,
}

struct BazNonCopyable {
  x: u32,
}

fn bar(mut b : Bar) -> Bar {
  b.name = "cat".to_string();
  b
}

fn foo(mut s : String) -> String {
  s.push_str("cat");
  s
}

fn baz(mut b: BazCopyable) -> BazCopyable {
  b.x = 33;
  b
}

fn baz2(mut b: BazNonCopyable) -> BazNonCopyable {
  b.x = 33;
  b
}

fn main() {
  {  
    let mut s0 = "foo".to_string();
    let _s1 = s0; // transfer ownership
    // let x0 = s0; // won't compile
    s0 = "bar".to_string();
    let mut s2 = foo(s0); // transfer ownership
    println!("{:?}", s2);
    // let x1 = s0; // won't compile
  
    let b0 = bar(Bar{name: "booya".to_string()});
    println!("{:?}", b0.name);
  
    let mut counter = 0;
    while counter < 10 {
      // foo(s2); // won't compile
      //     ^^ value moved here in previous iteration of loop
      s2 = foo(s2); // ownership is transfered in each loop
      counter += 1;
    }
  }

  {
    let v = vec![1, 2, 3];
    let mut w = vec![];
    for mut value in v {
      // ^^ why 'mut' here? removing it doesn't seem to change anything
      w.push(value);
    }
    println!("w: {:?}", w);
  }

  {
    let b0 = BazCopyable{x: 0};
    let b1 = baz(b0);
    println!("b0.x: {:?}, b1.x: {:.}", b0.x, b1.x);
    // Can still use b0 as it implements the Copy trait, thus ownership hasn't been
    // transfered

    // VS

    let b2 = BazNonCopyable{x: 0};
    let b3 = baz2(b2);
    println!("b3.x: {:.}", b3.x);
    // Can't use b2 as it has been moved by baz
  }

  {
    let s: Rc<String> = Rc::new("foo".to_string());
    let _s1 = s.clone();
    println!("Refcount = {:?}", std::rc::Rc::strong_count(&s));
  }

  {
    let mut x = 42;
    let y = x;
    let z = x;
  }
}
