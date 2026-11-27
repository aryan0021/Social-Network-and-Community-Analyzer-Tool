
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_USERS 100
#define MAX_POSTS 100
#define MAX_POST_LEN 256
#define MAX_ENGAGEMENT 1000
#define INF INT_MAX
typedef struct {
 int id;
 char name[16];
 int followers;

}
 User;
typedef struct {
 int id;
 char text[MAX_POST_LEN];

}
 Post;
typedef struct {
 int sum, l, r;

}
 Seg;
typedef struct {
 int u, v, w;

}
 Edge;
User users[MAX_USERS];
 int userCount = 0;
Post posts[MAX_POSTS];
 int postCount = 0;
int engagement[MAX_ENGAGEMENT], engagementCount = 0;
int graph[MAX_USERS][MAX_USERS];
int parent[MAX_USERS];
long cmpCount = 0, stepCount = 0;
void addEdge(int u, int v, int w) {
 if (u >= 0 && v >= 0 && u < MAX_USERS && v < MAX_USERS) graph[u][v] = graph[v][u] = w;

}
int loadDataset(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
 printf("Could not open dataset: %s\n", filename);
 return 0;

    }
    char line[512];
    userCount = postCount = engagementCount = 0;
    memset(graph, 0, sizeof(graph));
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (!line[0] || !strncmp(line, "record_type", 10)) continue;
        char *type = strtok(line, ",");
        char *f1 = strtok(NULL, ",");
        char *f2 = strtok(NULL, ",");
        char *f3 = strtok(NULL, ",");
        if (!type) continue;
        if (!strcmp(type, "USER") && f1 && f2 && f3 && userCount < MAX_USERS) {
            users[userCount].id = atoi(f1);
            snprintf(users[userCount].name, sizeof(users[userCount].name), "%s", f2);
            users[userCount].followers = atoi(f3);
            userCount++;

        }
 else if (!strcmp(type, "EDGE") && f1 && f2 && f3) {
            addEdge(atoi(f1), atoi(f2), atoi(f3));

        }
 else if (!strcmp(type, "ENGAGEMENT") && f2 && engagementCount < MAX_ENGAGEMENT) {
            engagement[engagementCount++] = atoi(f2);

        }
 else if (!strcmp(type, "POST") && f1 && f2 && postCount < MAX_POSTS) {
            posts[postCount].id = atoi(f1);
            snprintf(posts[postCount].text, sizeof(posts[postCount].text), "%s", f2);
            postCount++;

        }

    }
    fclose(fp);
    if (userCount == 0) {
 printf("Dataset has no users.\n");
 return 0;

    }
    int edgeCount = 0;
    for (int i = 0; i < userCount; i++) 
    for (int j = i + 1; j < userCount; j++) 
    if (graph[i][j]) 
    edgeCount++;
    printf("Dataset loaded: %d users, %d connections, %d engagement records, %d posts.\n", userCount, edgeCount, engagementCount, postCount);
    return 1;
}
/* 1. Sorting */
void mergeSort(User a[], User t[], int l, int r) {
    stepCount++;
 if (l >= r) return;
    int m = l + (r-l)/2;
 mergeSort(a,t,l,m);
 mergeSort(a,t,m+1,r);
    int i=l,j=m+1,k=l;
    while(i<=m && j<=r) {
 cmpCount++;
 t[k++] = (a[i].followers <= a[j].followers) ? a[i++] : a[j++];

    }
    while(i<=m) t[k++]=a[i++];
    while(j<=r) t[k++]=a[j++];
    for(k=l;k<=r;k++) a[k]=t[k];
}
void swapUser(User *x, User *y) {
 User z=*x;
 *x=*y;
 *y=z;

}
void quickSort(User a[], int l, int r) {
    stepCount++;
 if(l>=r) return;
    swapUser(&a[l+(r-l)/2],&a[r]);
 int pivot=a[r].followers,i=l;
    for(int j=l;j<r;j++){
 cmpCount++;
 if(a[j].followers<pivot) swapUser(&a[i++],&a[j]);

    }
    swapUser(&a[i],&a[r]);
 quickSort(a,l,i-1);
 quickSort(a,i+1,r);
}
void printLeaderboard(User a[], int n) {
 printf("\n  Rank  Name      Followers\n");
 for(int i=n-1,rank=1;i>=0;i--,rank++)
  printf("  %-5d %-9s %d\n",rank,a[i].name,a[i].followers);

}
/* 2. Binary search */
int bsIter(User a[], int n, int key) {
 int lo=0,hi=n-1;
 while(lo<=hi){
 stepCount++;
 int m=lo+(hi-lo)/2;
 if(a[m].followers==key)return m;
 if(a[m].followers<key)lo=m+1;
        else hi=m-1;
    }
    return -1;

}
int bsRec(User a[], int lo, int hi, int key) {
 stepCount++;
 if(lo>hi)return -1;
 int m=lo+(hi-lo)/2;
 if(a[m].followers==key)return m;
 if(a[m].followers<key)return bsRec(a,m+1,hi,key);
 return bsRec(a,lo,m-1,key);

}
/* 3. Maximum subarray, divide and conquer */
Seg maxCross(int a[],int l,int m,int r){
    int s=0,ls=INT_MIN,li=m,rs=INT_MIN,ri=m+1;
    for(int i=m;i>=l;i--){
        s+=a[i];
        if(s>ls){
            ls=s;
            li=i;
        }
    }
    s=0;
    for(int i=m+1;i<=r;i++){
        s+=a[i];
        if(s>rs){
            rs=s;
            ri=i;
        }
    }
    Seg z={
        ls+rs,li,ri
    }
    ;
    return z;
}
Seg maxSub(int a[],int l,int r){
    stepCount++;
    if(l==r){
        Seg z={
            a[l],l,l
        }
        ;
        return z;
    }
    int m=l+(r-l)/2;
    Seg x=maxSub(a,l,m),y=maxSub(a,m+1,r),z=maxCross(a,l,m,r);
    if(x.sum>=y.sum&&x.sum>=z.sum)return x;
    if(y.sum>=x.sum&&y.sum>=z.sum)return y;
    return z;
}
/* 4. Dijkstra */
void printPath(int prev[],int v){
    if(prev[v]!=-1){
        printPath(prev,prev[v]);
        printf(" -> ");
    }
    printf("%s",users[v].name);
}
void dijkstra(int src,int dst){
    int dist[MAX_USERS],prev[MAX_USERS],done[MAX_USERS]={
        0
    }
    ;
    for(int i=0;i<userCount;i++){
        dist[i]=INF;
        prev[i]=-1;
    }
    dist[src]=0;
 for(int c=0;c<userCount;c++){
        stepCount++;
        int u=-1;
        for(int i=0;i<userCount;i++){
            stepCount++;
            if(!done[i]&&dist[i]!=INF&&(u==-1||dist[i]<dist[u]))u=i;
        }
        if(u==-1)break;
        done[u]=1;
        for(int v=0;v<userCount;v++){
            stepCount++;
            if(graph[u][v]&&!done[v]&&dist[u]+graph[u][v]<dist[v]){
                dist[v]=dist[u]+graph[u][v];
                prev[v]=u;
            }
        }
    }
 if(dist[dst]==INF){
        printf("  No path exists.\n");
        return;
    }
    printf("  Lowest-cost influence path (total cost %d): ",dist[dst]);
    printPath(prev,dst);
    printf("\n");
}
/* 5. Kruskal MST */
int findSet(int x){
    return parent[x]==x?x:(parent[x]=findSet(parent[x]));
}
int cmpEdge(const void *a,const void *b){
    return ((Edge*)a)->w-((Edge*)b)->w;
}
void kruskal(void){
    Edge e[MAX_USERS*MAX_USERS];
    int m=0,total=0,used=0;
    for(int i=0;i<userCount;i++)for(int j=i+1;j<userCount;j++)if(graph[i][j]){
        e[m++]=(Edge){
            i,j,graph[i][j]
        }
        ;
    }
    qsort(e,m,sizeof(Edge),cmpEdge);
    for(int i=0;i<userCount;i++)parent[i]=i;
    printf("  Edges chosen for community backbone:\n");
    for(int i=0;i<m&&used<userCount-1;i++){
        stepCount++;
        int a=findSet(e[i].u),b=findSet(e[i].v);
        if(a!=b){
            parent[a]=b;
            printf("    %s - %s (cost %d)\n",users[e[i].u].name,users[e[i].v].name,e[i].w);
            total+=e[i].w;
            used++;
        }
    }
    printf("  Total cost: %d\n",total);
}
/* 6. 0/1 Knapsack */
void knapsack(int budget){
    const char *name[]={
        "Reel Ad","Story Ad","Influencer Collab","Banner","Hashtag Push","Live Event"
    }
    ;
    int cost[]={
        20,10,40,15,12,35
    }
    ,reach[]={
        50,25,120,30,28,90
    }
    ,n=6;
    static int dp[7][101];
    for(int i=0;i<=n;i++)for(int b=0;b<=budget;b++){
        stepCount++;
        if(i==0||b==0)dp[i][b]=0;
        else{
            dp[i][b]=dp[i-1][b];
            if(cost[i-1]<=b&&dp[i-1][b-cost[i-1]]+reach[i-1]>dp[i][b])dp[i][b]=dp[i-1][b-cost[i-1]]+reach[i-1];
        }
    }
    printf("  Best reach: %dk people. Campaigns chosen:\n",dp[n][budget]);
    for(int i=n,b=budget;i>0;i--)if(dp[i][b]!=dp[i-1][b]){
        printf("    %s (cost %d)\n",name[i-1],cost[i-1]);
        b-=cost[i-1];
    }
}
/* 7. Naive and KMP keyword search */
int naiveSearch(const char*t,const char*p){
    int n=(int)strlen(t),m=(int)strlen(p),found=0;
    for(int i=0;i+m<=n;i++){
        stepCount++;
        int j=0;
        while(j<m){
            cmpCount++;
            if(t[i+j]!=p[j])break;
            j++;
        }
        if(j==m)found++;
    }
    return found;
}
int kmpSearch(const char*t,const char*p){
    int n=(int)strlen(t),m=(int)strlen(p),found=0;
    if(m==0)return 0;
    int lps[100]={
        0
    }
    ;
    for(int i=1,len=0;i<m;){
        if(p[i]==p[len])lps[i++]=++len;
        else if(len)len=lps[len-1];
        else lps[i++]=0;
    }
    for(int i=0,j=0;i<n;){
        cmpCount++;
        if(t[i]==p[j]){
            i++;
            j++;
        }
        if(j==m){
            found++;
            j=lps[j-1];
        }
        else if(i<n&&t[i]!=p[j]){
            if(j)j=lps[j-1];
            else i++;
        }
    }
    return found;
}
/* 8. LCS */
int lcs(const char*a,const char*b){
    int n=(int)strlen(a),m=(int)strlen(b);
    if(n>100||m>100){
        printf("  Posts too long for this demo's LCS table.\n");
        return -1;
    }
    static int dp[101][101];
    for(int i=0;i<=n;i++)for(int j=0;j<=m;j++){
        stepCount++;
        if(i==0||j==0)dp[i][j]=0;
        else if(a[i-1]==b[j-1])dp[i][j]=dp[i-1][j-1]+1;
        else dp[i][j]=dp[i-1][j]>dp[i][j-1]?dp[i-1][j]:dp[i][j-1];
    }
    return dp[n][m];
}
/* 9. Greedy activity selection */
void activitySelection(void){
    int start[]={
        1,3,0,5,8,5
    }
    ,finish[]={
        2,4,6,7,9,9
    }
    ,n=6,chosen=0,last=-1;
    printf("  Selected engagement slots (non-overlapping):\n");
    for(int i=0;i<n;i++){
        stepCount++;
        if(start[i]>=last){
            printf("    Slot %d: %d-%d\n",i+1,start[i],finish[i]);
            last=finish[i];
            chosen++;
        }
    }
    printf("  Maximum compatible slots: %d | steps: %ld\n",chosen,stepCount);
}
/* 10. Benchmark */
void benchmark(int n){
    User*a=malloc((size_t)n*sizeof(User)),*b=malloc((size_t)n*sizeof(User)),*t=malloc((size_t)n*sizeof(User));
    if(!a||!b||!t){
        printf("  Out of memory.\n");
        free(a);
        free(b);
        free(t);
        return;
    }
    for(int i=0;i<n;i++){
        a[i].id=i;
        strcpy(a[i].name,"user");
        a[i].followers=rand()%1000000;
        b[i]=a[i];
    }
    clock_t s;
    double tm,tq;
    cmpCount=0;
    s=clock();
    mergeSort(a,t,0,n-1);
    tm=(double)(clock()-s)/CLOCKS_PER_SEC;
    long cm=cmpCount;
    cmpCount=0;
    s=clock();
    quickSort(b,0,n-1);
    tq=(double)(clock()-s)/CLOCKS_PER_SEC;
    long cq=cmpCount;
    printf("\n  n = %d users\n", n);
 printf("  %-12s %-12s %-14s\n", "Algorithm", "Time (s)", "Comparisons");
    printf("  %-12s %-12.4f %-14ld\n","Merge Sort",tm,cm);
    printf("  %-12s %-12.4f %-14ld\n","Quick Sort",tq,cq);
    printf("  Merge Sort: O(n log n) always; O(n) extra memory.\n  Quick Sort: O(n log n) average; O(n^2) worst case.\n");
    free(a);
    free(b);
    free(t);
}
int readInt(const char*prompt){
    int x;
    printf("%s",prompt);
    while(scanf("%d",&x)!=1){
        int c;
        while((c=getchar())!='\n'&&c!=EOF){
        }
        printf("  Invalid input, try again: ");
    }
    return x;
}
void menu(void){
    printf("\n=========== SocialNet - Community Influence Analyzer ===========\n 1. Influencer leaderboard (Merge vs Quick Sort)\n 2. Search follower count (Iterative vs Recursive)\n 3. Best engagement streak (Divide & Conquer)\n 4. Lowest-cost connection path (Dijkstra)\n 5. Community network backbone (Kruskal)\n 6. Campaign reach planner (0/1 Knapsack)\n 7. Post keyword search (Naive vs KMP)\n 8. Content similarity (LCS)\n 9. Engagement slot planning (Greedy Activity Selection)\n10. Sorting performance benchmark\n 0. Exit\n");
}
int main(void){
    srand((unsigned)time(NULL));
    if(!loadDataset("socialnet_dataset.csv"))return 1;
    int choice;
    do{
        menu();
        choice=readInt("Choose: ");
        switch(choice){
            case 1:{
                User a[MAX_USERS],b[MAX_USERS],t[MAX_USERS];
                memcpy(a,users,(size_t)userCount*sizeof(User));
                memcpy(b,users,(size_t)userCount*sizeof(User));
                stepCount=cmpCount=0;
                mergeSort(a,t,0,userCount-1);
                long sm=stepCount,cm=cmpCount;
                stepCount=cmpCount=0;
                quickSort(b,0,userCount-1);
                long sq=stepCount,cq=cmpCount;
                printLeaderboard(a,userCount);
                printf("\n  Comparisons -> Merge: %ld, Quick: %ld\n  Recursive calls -> Merge: %ld, Quick: %ld\n",cm,cq,sm,sq);
                break;
            }
            case 2:{
                User a[MAX_USERS],t[MAX_USERS];
                memcpy(a,users,(size_t)userCount*sizeof(User));
                mergeSort(a,t,0,userCount-1);
                int key=readInt("  Enter exact follower count: ");
                stepCount=0;
                int i1=bsIter(a,userCount,key);
                long si=stepCount;
                stepCount=0;
                int i2=bsRec(a,0,userCount-1,key);
                long sr=stepCount;
                printf("  Search iterations/calls -> Iterative: %ld, Recursive: %ld\n",si,sr);
                if(i1==-1)printf("  No user has %d followers.\n",key);
                else printf("  Found: %s (iterative index %d, recursive index %d)\n",a[i1].name,i1,i2);
                break;
            }
            case 3:{
                if(!engagementCount){
                    printf("  No engagement data loaded.\n");
                    break;
                }
                stepCount=0;
                Seg s=maxSub(engagement,0,engagementCount-1);
                printf("  Best growth streak: record %d to %d, total gain = %d\n",s.l+1,s.r+1,s.sum);
                printf("  Recursive calls/steps: %ld\n",stepCount);
                break;
            }
            case 4:{
                printf("  Users: ");
                for(int i=0;i<userCount;i++)printf("%d=%s ",users[i].id,users[i].name);
                printf("\n");
                int s=readInt("  Source id: "),d=readInt("  Destination id: ");
                if(s<0||s>=userCount||d<0||d>=userCount)printf("  Invalid user id.\n");
                else{
                    stepCount=0;
                    dijkstra(s,d);
                    printf("  Selection/relaxation steps: %ld\n",stepCount);
                }
                break;
            }
            case 5:stepCount=0;
            kruskal();
            printf("  Edge-processing steps: %ld\n",stepCount);
            break;
            case 6:{
                int b=readInt("  Enter budget (1-100): ");
                if(b<1||b>100)printf("  Budget out of range.\n");
                else{
                    stepCount=0;
                    knapsack(b);
                    printf("  DP table cells processed: %ld\n",stepCount);
                }  
                break;
            }
            case 7:{
                if(!postCount){
                    printf("  No posts loaded.\n");
                    break;
                }
                char combined[4096]="";
                for(int i=0;i<postCount;i++){
                    if(strlen(combined)+strlen(posts[i].text)+2>=sizeof(combined))break;
                    strcat(combined,posts[i].text);
                    strcat(combined," ");
                }
                char pat[100];
                printf("  Posts: %s\n  Keyword to search: ",combined);
                if(scanf("%99s",pat)!=1)break;
                stepCount=cmpCount=0;
                int f1=naiveSearch(combined,pat);
                long c1=cmpCount,s1=stepCount;
                stepCount=cmpCount=0;
                int f2=kmpSearch(combined,pat);
                long c2=cmpCount,s2=stepCount;
                printf("  Naive: %d match(es), %ld comparisons, %ld alignments\n  KMP: %d match(es), %ld comparisons, %ld steps\n",f1,c1,s1,f2,c2,s2);
                break;
            }
            case 8:{
                if(postCount<2){
                    printf("  Need at least two posts for LCS.\n");
                    break;
                }
                int p1=0,p2=1;
                printf("  Comparing posts:\n  Post 1: %s\n  Post 2: %s\n",posts[p1].text,posts[p2].text);
                stepCount=0;
                int l=lcs(posts[p1].text,posts[p2].text);
                if(l>=0){
                    int longer=(int)strlen(posts[p1].text)>(int)strlen(posts[p2].text)?(int)strlen(posts[p1].text):(int)strlen(posts[p2].text);
                    printf("  LCS length = %d, similarity = %.1f%%\n",l,longer?100.0*l/longer:0.0);
                }
                printf("  DP cells processed: %ld\n",stepCount);
                break;
            }
            case 9:stepCount=0;
            activitySelection();
            break;
            case 10:{
                int n=readInt("  Number of users to benchmark (e.g. 100000): ");
                if(n<1||n>5000000)printf("  Choose between 1 and 5,000,000.\n");
                else benchmark(n);
                break;
            }
            case 0:printf("  Goodbye!\n");
            break;
            default:printf("  Invalid choice.\n");
        }
    }
    while(choice!=0);
    return 0;
}
