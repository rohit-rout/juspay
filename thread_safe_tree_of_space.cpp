#include<bits/stdc++.h>
using namespace std;


//-----------
bool  checkSuspendList(){
   return true ;
}
//---------
struct Node{
 string val;
 bool isResourceInUse;
 set<Node*> des_lock_set;
  bool is_lock;
  Node *parent;

  int uid=-1;

  vector<Node*> children;

  Node(string node_val, Node * par){
    this->val=node_val;
    des_lock_set.clear();
    is_lock=false;
    parent=par;
    uid=-1;
  }
};

class CustomSemaphore{

    bool semaphore;
    public:
    CustomSemaphore(){
        semaphore=true;
    }
    void wait(int op_type, int op_key){
       if(semaphore){

        semaphore=false;

       }else{
        // put the process into the suspend list
         
       }
    }

    void signal(int op_type, int op_key){
        
      if(/* if some process is present in the suspend list awake() that process*/ checkSuspendList()){
       semaphore=false;
      }else
        semaphore=true;
    }
};

CustomSemaphore *customSemaphore=new CustomSemaphore();

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
    cout<<"locked_nodes in descendents ";
    for(auto a: root->des_lock_set){
      cout<<a->val<<" ";
    }
    cout<<endl;

	cout << "Child: \n";
	for(auto k : root->children){ 
		cout << k->val<<" locked_status "<<k->is_lock << " uid:" << k->uid << "\n";

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
       void inform_ancestors(Node *par, Node *locked_node,int type){
        while(par){
          if(type==1)
          par->des_lock_set.insert(locked_node);
          else
          par->des_lock_set.erase(locked_node);
          par=par->parent;
        }
       }

       bool check_ances_unlock_status(Node * root){
        if(root==NULL)
        return true;

        if(root->is_lock)
        return false;
        return check_ances_unlock_status(root->parent);
       }

       void blockResources(Node *root){
        if(root==NULL)
        return ;
        root->isResourceInUse=true;
        return blockResources(root->parent);
       }

       void unblockResources(Node * root){
         if(root==NULL)
        return ;
        root->isResourceInUse=false;
        return unblockResources(root->parent);
       }
       
  //------------------------------------

      bool simpleLockOperation(string cur_val, int lock_id){

        Node * cur_node=node_map[cur_val];

        // CHECKING THE BASE CASE
        if(cur_node->des_lock_set.size() || cur_node->is_lock||  !check_ances_unlock_status(cur_node->parent))
          return false;
        
        cur_node->is_lock=true;
        cur_node->uid=lock_id;

      //  INFORM ONY THE ACESTORS ABOUT THIS LOCKED NODE
        inform_ancestors(cur_node->parent,cur_node,1);
        
        return true;     
    }

      bool simpleUnlockOperation(string cur_val, int unlock_id){

       Node *cur_node=node_map[cur_val];

      //  BASE CONDITION CHECK
       if(!cur_node->is_lock || cur_node->uid!=unlock_id)
       return false;

       cur_node->is_lock=false;
       cur_node->uid=-1;
      
      // INFORM THE ANCESTORS ABOUT THIS UNLOCKED NODE
       inform_ancestors(cur_node->parent,cur_node,0);

       return true;
    }

    bool lock_operation(string cur_val, int lock_id){

        Node * cur_node=node_map[cur_val];

       //--------------IF RESOURCE IS NOT IN USE WE ARE SAFE TO CHECK THE BASE CONDITON --------------------------------
        if(!cur_node->isResourceInUse){
        // CHECKING THE BASE CASE
        if(cur_node->des_lock_set.size() || cur_node->is_lock)
          return false;
        }
      //  ----------------------------------------------------------------------------------------------------------------

      customSemaphore->wait(1,lock_id);
      //-----------------------CRITICAL SECTION START----------------------------
        
        //block all the nodes from this curnode to root path.
        blockResources(cur_node);
        bool ok=true;
        if(cur_node->des_lock_set.size() || cur_node->is_lock || !check_ances_unlock_status(cur_node->parent)) // base condition check 
            ok=false;
        else{
        cur_node->is_lock=true;   
        cur_node->uid=lock_id;

      //  INFORM ONLY THE ACESTORS ABOUT THIS LOCKED NODE
        inform_ancestors(cur_node->parent,cur_node,1);
        }

        //unblock all the nodes fro this cur node to root path
        unblockResources(cur_node);
      //-----------------------CRITICAL SECTION END------------------------------------------
      customSemaphore->signal(1,lock_id);

        return ok;     
    }

    bool unlock_operation(string cur_val, int unlock_id){

       Node *cur_node=node_map[cur_val];

         //--------------IF RESOURCE IS NOT IN USE WE ARE SAFE TO CHECK THE BASE CONDITON --------------------------------
        if(!cur_node->isResourceInUse){
        // CHECKING THE BASE CASE
           if(!cur_node->is_lock || cur_node->uid!=unlock_id)
          return false;
        }
      //  ----------------------------------------------------------------------------------------------------------------
        

          customSemaphore->wait(2,unlock_id);
      //-----------------------CRITICAL SECTION START----------------------------
        
        //block all the nodes from this curnode to root path.
        blockResources(cur_node);
        bool ok=true;
       //  BASE CONDITION CHECK
       if(!cur_node->is_lock || cur_node->uid!=unlock_id)
       ok=false;

       cur_node->is_lock=false;
       cur_node->uid=-1;
      
      // INFORM THE ANCESTORS ABOUT THIS UNLOCKED NODE
       inform_ancestors(cur_node->parent,cur_node,0);

        //unblock all the nodes fro this cur node to root path
        unblockResources(cur_node);
      //----------------------CRITICAL SECTION END-------------------------------------------
      customSemaphore->signal(2,unlock_id);

       return ok;
    }

    bool upgrade_operation(string cur_val, int upgrade_id){

       Node *cur_node=node_map[cur_val];

      //  BASE CONDITION CHECK
      //----- If Resource is not in use we are safe to check some base cases without blocking the resources------
      if(!cur_node->isResourceInUse)
       if(cur_node->is_lock)
       return false;
      

         customSemaphore->wait(3,upgrade_id);
      //-----------------------CRITICAL SECTION START----------------------------
        
        //block all the nodes from this curnode to root path.
        blockResources(cur_node);
        bool ok=true;

       //BASE CONDTION CHECK---------
       if(cur_node->is_lock || !check_ances_unlock_status(cur_node->parent))
         ok=false;
      // Check if all the locked node in descendents are locked by the same id or not
       for(auto &child: cur_node->des_lock_set){
        if(child->is_lock and child->uid!=upgrade_id)
         ok=false;
        }
       //---------------------------
         
      if(ok){
      // UNLOCKING ALL THE DESCENDENTS
       set<Node*> unlock_these_nodes=cur_node->des_lock_set;
      
       for(auto &node : unlock_these_nodes){
        simpleUnlockOperation(node->val,upgrade_id);
       }
      // LOCK THIS CURRENT NODE
       simpleLockOperation(cur_node->val,upgrade_id);
      }

        
        unblockResources(cur_node);
      //-----------------------CRITICAL SECTION END------------------------------------------
      customSemaphore->signal(3,upgrade_id);

       return ok;
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
            //  printTree(root);
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
