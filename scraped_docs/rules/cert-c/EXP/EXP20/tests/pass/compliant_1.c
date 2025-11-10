LinkedList bannedUsers;

int is_banned(User usr) {
  int x = 0;

  Node cur_node = (bannedUsers->head);

  while(cur_node != NULL) {
    if (strcmp((char *)cur_node->data, usr->name)==0) {
      x++;
    }
    cur_node = cur_node->next;
  }

  return x;
}

void processRequest(User usr) {
  if (is_banned(usr) != 0) {
    return;
  }
  serveResults();
}