#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */
int beargit_init(void) {
  fs_mkdir('.beargit');
  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);
  FILE *fprev = fopen(".beargit/.prev", "w");
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  return 0;
}



/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen('.beargit/.index', 'r');
  FILE* fnewinex = fopen('.beargit/.newindex', 'w');
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)){
    strtok(line, "\n");
    if(strcmp(line, filename)==0){
      fprintf(stderr, ">> ERROR: File %s already added\n", filename);
      fcloes(fnewindex);
      fclose(findex);
      fs_rm(fnewindex);
      return 3;
    }
    fprintf(fnewindex, "%s\n", line);
  }
  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);
  fs_rm('.beargit/.index');
  fs_mv('.beargit/.newindex', '.beargit/.index');
  return 0;
  
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  int count = 0;
  fprintf(stdout, "Tracked files:\n\n");
  FILE* findex = fopen(".beargit/.index", 'r');
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)){
    strtok(line, "\n");
    fprintf(stdout, " <%s>\n", line);
    count += 1;
  }
  fprintf(stdout, "\n%d files total\n", count);
  fclose(findex);
  return 0;
}

/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", 'r');
  FILE* fnewindex = fopen(".beargit/.newindex", 'w');
  int flag = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)){
    strtok(line, '\n');
    if(strcmp(line, filename)===0){
      flag = 1;
      continue;
    }
    fprintf(fnewindex, "%s\n", line);
  }
  fclose(findex);
  fclose(fnewindex);
  if(flag==1){
    fs_mv(".beargit/.newindex", ".beargit/.index");
    return 0;
  }
  else{
    fs_rm(".beargit/.newindex");
    fprintf(stderr, "ERROR: File %s not tracked\n", filename);
    return 1;
  }
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg){
  if(strstr(msg, go_bears)!= NULL){
    return 1;
  }
  return 0;
}
// int is_commit_msg_ok(const char* msg) {
//   /* COMPLETE THE REST */
//   int flipper = 0;
//   char bears_count[] = "GO BEARS!";
//   int i;
//   for (i = 0; *msg != '\0'; msg++) {
//     if (*msg == bears_count[i]) {
//       flipper = 1;
//       if (bears_count[i + 1] == '\0') {
//         return 1;
//       }
//       i++;
//     } else if (flipper == 1) {
//       flipper = 0;
//       i = 0;
//     }
//   }
//   return 0;
// }



int beargit_commit(const char* msg) {
  //First, check whether the commit string contains "GO BEARS!". If not, display an error message.
  if(!is_commit_msg_ok(msg)){
    fprintf(stderr, "ERROR: Message must contain "\"GO BEARS!\"");
    return 1;
  }

  //Read the ID of the previous last commit from .beargit/.prev
  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);

  //Generate the next ID (newid) in such a way that:
  next_commit_id(commit_id);

  //Generate a new directory .beargit/<newid> and copy .beargit/.index, .beargit/.prev and all tracked files into the directory.
  char* new_id_dir = malloc((strlen(".beargit/") + strlen(commit_id) + 1)*sizeof(char));
  sprintf(new_id_dir, "%s/%s", ".beargit", commit_id);
  fs_mkdir(new_id_dir);

  char* new_id_prev = malloc((strlen(new_id_dir) + strlen("/.prev") + 1)*sizeof(char));
  sprintf(new_id_prev, "%s/%s", new_id_dir, ".prev");
  char* new_id_index = malloc((strlen(new_id_dir) + strlen("/.index") + 1)*sizeof(char));
  sprintf(new_id_index, "%s/%s", new_id_dir, ".index");
  fs_cp('.beargit/.index', new_id_index);
  fs_cp('.beargit/.prev', new_id_prev);
  FILE* findex = fopen(".beargit/.index", 'r');
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)){
    strtok(line, "\n");
    char* file_name = malloc((strlen(line) + strlen(new_id_dir) + 1));
    sprintf(file_name, "%s/%s", new_id_dir, line);
    fs_cp(line, file_name);
  }
  fclose(findex);

  //Store the commit message (<msg>) into .beargit/<newid>/.msg
  char* msg_filename = malloc((strlen(new_id_dir) + strlen("/.msg") + 1)*sizeof(char));
  write_string_to_file(msg_filename, msg);

  //Write the new ID into .beargit/.prev.
  write_string_to_file(".beargit/.prev", commit_id);
  fclose(msg_file);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 * 
 */

int beargit_log(int limit) {
  /* COMPLETE THE REST */
  count = 0;
  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  while(count<limit && strlen(commit_id)>0){
    char* commit_dir = malloc((strlen(".beargit/") + strlen(commit_id) + 1) * sizeof(char));
    if(!fs_check_dir_exists(commit_dir)){
      free(commit_dir);
      break;
    }
    count ++;
    fprintf(stdout, "\n");
    fprintf(stdout, "commit %s\n", commit_id);
    free(commit_dir);
    
    char* msg = malloc((strlen(".beargit/.msg/") + strlen(commit_id) + 1) * sizeof(char));
    sprintf(msg, ".beargit/%s/.msg",  commit_id);
    char message[MSG_SIZE+1];
    read_string_from_file(msg, message, MSG_SIZE); 
    fprintf(stdout, "    %s", message);
    free(msg);

    char* prev = malloc((strlen(".beargit/.prev/") + strlen(commit_id) + 1) * sizeof(char));
    sprintf(prev, ".beargit/%s/.prev",  commit_id);
    read_string_from_file(prev, commit_id, COMMIT_ID_SIZE);
    free(prev);
  }
  if(count==0){
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }
  else{
    fprintf(stdout, "\n");
    return 0;
  }
}


const char* digits = "61c";

void next_commit_id(char* commit_id) {
  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  char commit_id_branch[COMMIT_ID_BRANCH_BYTES];
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  int branch_index = get_branch_number(current_branch);
  for(int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++){
    commit_id[i] = digits[branch_index % 3];
    branch_index /= 3;
  }


  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_part1(commit_id + COMMIT_ID_BRANCH_BYTES);
}

void next_commit_id_part1(char* commit_id) {
  /* COMPLETE THE REST */
  char* new_id = commit_id;
  while(*new_id!='\0'){
    if(*new_id=='0'){
      *new_id = '6';
      *new_id++;
      continue;
    }
    if(*new_id=='6'){
      *new_id = '1';
      break;
    }else if(*new_id=='1'){
      *new_id = 'c';
      break;
    }else if(*new_id=='c'){
      *new_id = '6';
      new_id++;
      continue;
    }
    else{
      *new_id = '6';
      new_id++;
      continue;
    }
  }
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");
  int counter = 0 ;
  int branch_index = -1;
  char line[BRANCHNAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)){
    strtok(line, "\n");
    if(strcmp(branch_name, line)==0){
      branch_index = counter;
      break;
    }
    counter++;
  }
  fclose(branch_index);
  return branch_index;
}

/* beargit branch
 *
 * See "Step 5" in the homework 1 spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */
  FILE* fbranches = fopen(".beargit/.branches", "r");
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  char branch[BRANCHNAME_SIZE];
  while(gets(fbranches, sizeof(branch), branch)){
    strtok(branch, "\n");
    if(strcmp(branch, current_branch)==0){
      fprintf(stdout, "* %s\n", branch);
    }
    else{
      fprintf(stdout, "  %s\n", branch);
    }
  }
  fclose(fbranches);
  return 0;
}

/* beargit checkout
 *
 * See "Step 6" in the homework 1 spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
  if(strcmp(commit_id, "0000000000000000000000000000000000000000")!=0){
    FILE* findex = fopen(".beargit/.index", "r");
    char line[FILENAME_SIZE];
    while(fgets(findex, sizeof(line), line)){
      strtok(line, "\n");
      fs_rm(line);
    }
    fclose(findex);

    char* commit_id_dir = malloc((strlen(".beargit/" + strlen(commit_id) + 1) * sizeof(char));
    char* commit_id_index = malloc((sizeof(".beargit/") + strlen(commit_id) + strlen("/.index") + 1) * sizoef(char));
    sprintf(commit_id_dir, "%s/%s", ".beargit", commit_id);
    sprintf(commit_id_index, "%s/%s/%s", ".beargit", commit_id, ".index");
    fs_cp(commit_id_index, ".beargit/.index")
    FILE* commit_id_index  = fopen(commit_id_index, "r");
    char line[FILENAME_SIZE];
    while(fgets(commit_id_index, sizeof(line), line)){
      strtok(line, "\n");
      char* file_name = malloc((strlen("./beargit/") + strlen(commit_id) + strlen(line) + 1) * sizeof(char));
      sprintf(file_name, ".beargit/%s/%s", commit_id, line);
      fs_cp(file_name, line);
    }
    fclose(commit_id_index);
    write_string_to_file(".beargit/.prev", commit_id);
  
  }
  else{
    write_string_to_file(".beargit/.prev", commit_id);
    write_string_to_file(".beargit/.index", "\0");
  }
  return 0;

}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  if(strlen(commit_id)!=COMMIT_ID_BYTES){
    return 0;
  }
  int i = 0;
  while(commit_id[i]!='\0'){
    if(commit_id[i]!='6' || commit_id[i]!='1' || commit_id[i]!='c'){
      return 0;
    }
  }
  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  // Even if we cancel later, this is still ok.
  if(strlen(current_branch)){
    char* branch_name = malloc((strlen(".beargit/") + strlen("branch_") + strlen(current_branch) + 1)*sizeof(char));   
    sprintf(branch_name, ".beargit/branch_%s", current_branch);
    fs_cp(".beargit/.prev", branch_name);
  }


  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if(is_it_a_commit_id(arg)){
    char commit_id_dir[FILENAME_SIZE+100] = ".beargit/";
    strcat(commit_id_dir, arg);
    if(!fs_check_dir_exists(commit_id_dir)){
      fprintf(stderr, "Commit %s does not exist\n", arg);
      return 1;
    }

    // Set the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "\0");
    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;



  // Read branches file (giving us the HEAD commit id for that branch).


  // Check for errors.
  int branch_exists = get_branch_number(branch_name);
  if((branch_exists && new_branch)){
    fprintf(stderr, "A branch named %s already exists", branch_name);
    return 1;
  }
  if((!branch_exists && !new_branch)){
    fprintf(stderr, "No branch %s exists", branch_name);
    return 1;
  }


  // File for the branch we are changing into.
  char branch_head_file[BRANCHNAME_SIZE + 100];
  sprintf(branch_head_file, ".beargit/.branch_%s", arg);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if(new_branch){
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_head_file);
  }
  write_string_to_file(".beargit/.current_branch", branch_name);


  // Read the head commit ID of this branch.
  char branch_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_head_file, branch_commit_id, COMMIT_ID_SIZE);


  // Check out the actual commit.
  checkout_commit(branch_commit_id);
}
