#include<bits/stdc++.h>
using namespace std;

struct Node{
 string val;
  int  des_lock_count,anc_lock_count;
  bool is_lock;
  Node *parent;
  int uid=-1;

  vector<Node*> children;

  Node(string node_val, Node * par){
    this->val=node_val;
    anc_lock_count=0;
    des_lock_count=0;
    is_lock=false;
    parent=par;
    uid=-1;
  }

};

Node * buildTree(Node * root, Node * par, int& m ,int& n,int start, vector<string> &nodes){
  
  root->parent=par;
  queue<Node*> q;
  q.push(root);
  while(q.size()){
    Node * cur_root=q.front();
    q.pop();
    if(start==n)
    continue;
  for(int i=start;i<start+m and i<n; i++ ){
    Node *temp=new Node(nodes[i],cur_root);
    cur_root->children.push_back(temp);
    q.push(temp);
   }
   start=min(start+m,n);

  }

  return root;
}

void printTree(Node * root){
    cout << "Parent: " << root->val << "\n";
	cout << "Child: \n";
	for(auto k : root->children){
		cout << k->val << " anc_locked: " << k->anc_lock_count 
			<< " dec_locked:" << k->des_lock_count << " isLocked:" 
			<< k->is_lock << " uid:" << k->uid << "\n";
	}
	cout << "\n";
	for(auto k : root->children){
		printTree(k);
	}
}

class N_ary_Tree{
    private:
    Node * root;
    map<string,Node*> node_map;

    public:
    N_ary_Tree(Node * cur_root){
     root=cur_root;
     dfs_to_map(root);
    }

     //---- function to map the string value to node
    void dfs_to_map(Node *root){
        node_map[root->val]=root;
        for(auto child: root->children){
            dfs_to_map(child);
        }

    }

    //---------- middleware functions------
  
    void inform_descends(Node * root,int num){
       for(auto child: root->children){
        child->anc_lock_count+=num;
        inform_descends(child,num);
       }
    }
    void inform_ancestors(Node * root, int num){
        while(root){
            root->des_lock_count+=num;
            root=root->parent;
        }
    }

    bool verify_descends(Node *root,int id){
        bool isSatified=true;
        for(auto child: root->children){
            if(child->is_lock and child->uid!=id)
            return false;
            isSatified&=verify_descends(child,id);

        }
        return isSatified;
    }

    void unlock_all_descends(Node *root,int id){
      for(auto child: root->children)
      {
        if(child->is_lock)
        unlock_operation(child->val,id);
        
        unlock_all_descends(child,id);
      }
    }
  //------------------------------------
    

    bool lock_operation(string cur_val, int lock_id){
        Node * cur_node=node_map[cur_val];

        // base condition check 
        if(cur_node->is_lock || cur_node->des_lock_count || cur_node->anc_lock_count )
        return false;

        cur_node->is_lock=true;
        cur_node->uid=lock_id;
        
    //   Informing the parent about this locked node.
        Node * ptr=cur_node->parent;
        inform_ancestors(ptr,1);
     

    // Informing the descendents about this locked node.
         inform_descends(cur_node,1);
       
      return true;
        
    }

    bool unlock_operation(string cur_val, int unlock_id){

        Node * cur_node=node_map[cur_val];

        //  base condition check
        if(!cur_node->is_lock or cur_node->uid!=unlock_id)
        return false;
        
        // unlocking the current node
        cur_node->is_lock=false;
        cur_node->uid=-1;


         //   Informing the parent about this unlocked node.
        Node * ptr=cur_node->parent;
        inform_ancestors(ptr,-1);
     

        // Informing the descendents about this unlocked node.
         inform_descends(cur_node,-1);
       
         return true; 


    }

    bool upgrade_operation(string cur_val, int upgrade_id){
        Node *cur_node=node_map[cur_val];

        // base condition check 
        if(cur_node->is_lock || !cur_node->des_lock_count || cur_node->anc_lock_count )
         return false;

        // verify if descendents are locked by the same upgrade_id or not
        bool result=verify_descends(cur_node,upgrade_id);
        if(!result)
        return false;


       // since we are now going to upgrade this node so unlock the descendents
        unlock_all_descends(cur_node,upgrade_id);
        
        // lock this node
        lock_operation(cur_node->val,upgrade_id);

        return true;
    }
};


int main(){

    int n,m,q;
    cin>>n>>m>>q;

    vector<string> nodes(n);
    for(int i=0;i<n;i++){
        cin>>nodes[i];
    }
   Node *root=new Node(nodes[0],NULL);

   buildTree(root,NULL,m,n,1,nodes);

  //  printTree(root);

   N_ary_Tree *tree=new N_ary_Tree(root);

   for(int i=0;i<q;i++){
    int opcode;
    string node_val;
    int id;
    cin>>opcode>>node_val>>id;

    switch (opcode)
    {
            case 1:	if(tree->lock_operation(node_val,id)){
						cout << "true\n";
						//printTree(r);
					}else
						cout << "false\n";
					break;
			case 2:	if(tree->unlock_operation(node_val,id))
						cout << "true\n";
					else
						cout << "false\n";
					break;
			case 3:	if(tree->upgrade_operation(node_val,id)){
						cout << "true\n";
						//printTree(r);
					}else
						cout << "false\n";
					break;
          default:
          break;
    }


   }
  


}
