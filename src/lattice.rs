pub struct Node {
    /// Unique ID of the node
    pub id: u32,

    /// The element ID of the node
    /// TODO: Check if we really need to construct it first to get an elemen ID
    pub elem_id: Option<u32>,
}

pub struct Lattice {
    /// Counter for a unique index of the nodes
    index: u32,

    /// All nodes in the structure
    pub nodes: Vec<Node>,

    /// All edges in the structure
    pub edges: Vec<(u32, u32)>,
}

impl Lattice {
    pub fn new() -> Lattice {
        Lattice {
            index: 0,
            nodes: Vec::new(),
            edges: Vec::new(),
        }
    }

    pub fn add_node(&mut self, elem_id: Option<u32>) -> u32 {
        let node = Node {
            id: self.index,
            elem_id,
        };
        self.nodes.push(node);
        self.index += 1;
        self.index - 1
    }

    pub fn update_node(&mut self, id: u32, elem_id: u32) -> Option<u32> {
        if let Some(node) = self.nodes.iter_mut().find(|n| n.id == id) {
            node.elem_id = Some(elem_id);
            return Some(node.id);
        }
        None
    }

    pub fn add_edge(&mut self, from: u32, to: u32) -> usize {
        if self
            .edges
            .iter()
            .find(|(f, t)| *f == from && *t == to)
            .is_none()
            && self
                .edges
                .iter()
                .find(|(f, t)| *f == to && *t == from)
                .is_none()
        {
            self.edges.push((from, to));
        }
        self.edges.len()
    }
}
