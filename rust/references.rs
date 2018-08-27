use std::collections::HashMap;

type Table = HashMap<u32, Vec<String>>;

fn show_table(t : &Table) {
  for (k, v) in t {
    println!("{:?} => {:?}", k, v);
  }
}

fn show_table2(t : Table) -> Table {
  for (k, v) in &t {
    println!("{:?} <--> {:?}", k, v);
  }
  t
}

fn mutate_table(t: &mut Table) {
  for (_k, v) in t {
    v.sort();
  // ^ . operator implicitely dereferences
  }
}

fn mutate_u32(x: &mut u32) {
  *x = 33;
}

fn foo<'a>(xs: &'a[i32]) -> &'a i32{
  &xs[0]
}

fn main() {
  {
    let mut table = Table::new();
    table.insert(32, vec!["a".to_string()]);
    table.insert(34, vec!["z".to_string(), "b".to_string()]);
  
    show_table(&mut table);
    mutate_table(&mut table);
    show_table(&table);
    for (k, v) in &mut table {
      println!("{:?} {:?}", k, v);
      *v = vec!["c".to_string()];
    }
    table = show_table2(table);

    for (k, v) in table {
      println!("{:?} {:?}", k, v);
    }

    // wont' compile because the previous loop took ownership of elements
    // show_table(&table);
  }

  {
    let mut x = 42;
    mutate_u32(&mut x);
    println!("x={:?}", x);
  }

  {
    let mut x = 10;

    let r = &mut x;
    println!("{:?}", *r);
    *r = 33;
    println!("*r={:?}", *r);

    //  r = &y;
    //      ^^ types differ in mutability
    //
    // note: expected type `&mut {integer}`
    //         found type `&{integer}`
  }
  {
    let x = 10;
    let y = 20;

    let mut r = &x;

    println!("*r={:?}", *r);
    r = &y;
    println!("*r={:?}", *r);
  }

  {
    let x = 10;
    let y = 20;
    let z = 10;

    let rx = &x;
    let ry = &y;
    let rz = &z;

    assert!(rx < ry);
    assert!(rx == rz);
    assert!(!std::ptr::eq(rx, ry));
  }
  {
    let xs = vec![1,2,3];
    let _r = foo(&xs);
  }
}
