#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#include "memory/paddr.h"

enum {
  TK_NOTYPE = 256, TK_EQ=255, NUMBER=254,negative=253,
  sixteen=252,reg=251,deref=250,and=249,neq=248,PC=257,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  //被用于正则表达式编译，注意转义符号。
  {"0x[0-9a-f]+",sixteen},//十六进制数，要先判断
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\-", '-'},           //sub
  {"\\*",'*'},          //multiply
  {"/",'/'},            //divide
  {"[0-9]+",NUMBER},      //number
  {"\\(",'('},          //左括号
  {"\\)",')'},          //右括号
// {"\\-[0-9]+",negative},
  {"\\$\\p\\c",PC},
  {"\\$[a-z]+",reg},
  {"&&",and},
  {"!=",neq},
  
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

unsigned int calculate();

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}
//regcomp(regex_t *compiled,const char* pattern.int cflags) 
//编译正则表达式。et为regcomp返回值，函数执行成功则返回0。 
//这一部分编译完所有rules中的模式，存储在re[]中

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[500] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

//make_token 用于识别待求值表达式的token
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  memset(tokens,0,sizeof(tokens));

  while (e[position] != '\0' && e[position]!='\n') 
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) 
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) 
      {
        //判断条件，成功匹配,且只返回第一个匹配到的token
        char *substr_start = e + position; //匹配的字符串的起始位置
        int substr_len = pmatch.rm_eo;    //匹配的字符串的终止位置

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //   i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) 
        {
          case 256: break;
          default: 
            tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            nr_token++;
            break;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for(int i = 0;i < nr_token;i++)
  {
    if(tokens[i].type=='-'&&(i==0||(tokens[i-1].type!=NUMBER&&tokens[i-1].type!=')')))
    {
      tokens[i].type=negative;
      tokens[i].str[0]='-';
    }
    if(tokens[i].type=='*'&& (i == 0 || (tokens[i - 1].type != NUMBER &&tokens[i-1].type!=')')) )
    {
      tokens[i].type=deref;
    }
  }

  unsigned int ans=calculate(0,nr_token);
  //printf("%u\n",ans);
  
  return ans;
}
//得到tokens后计算求值,并输出

unsigned int calculate(int p,int q)
{
  if(p >= q)
  {
    printf("wrong");
    return 0;
  }
  int count=p,num_of_stack=0,sign[100]={0},tmp=0,i=0;
  unsigned int temp1=0,temp2=0,temp=0,stack[100]={0},val1=0,val2=0;
  bool flag=true;
  memset(sign,0,sizeof(sign));
  memset(stack,0,sizeof(stack));
  while(count < q)
  {
    count++;
    switch(tokens[count-1].type)
    {
      case 257:
          stack[num_of_stack++]=cpu.pc;
          break;
      case 253:
          tmp=0;
          for(i = count;i < q;i ++ )
          {
            if(tokens[i].type=='(')
            {
              tmp+=1;
            }
            else if(tokens[i].type==')')
            {
              tmp-=1;
            }
            if(tmp==0&&(tokens[i].type==NUMBER||tokens[i].type==')'))
            {
              tmp=i;
              break;
            }
          }
          temp=(-1U)*calculate(count,tmp+1);
          tokens[tmp].type=NUMBER;
          sprintf(tokens[tmp].str,"%u",temp);
          count=tmp;
          break;//处理非减法的-号
      case 255:
          tmp=0;
          for(i = count;i < q;i ++ )
          {
            if(tokens[i].type=='(')
            {
              tmp+=1;
            }
            else if(tokens[i].type==')')
            {
              tmp-=1;
            }
            if(tmp==0)
            {
              tmp=i;
              break;
            }
          }
          if(i == q)
              val2=calculate(count,q);
          else val2=calculate(count,tmp+1);//等号右侧的值
          
          val1=stack[num_of_stack-1];//计算左侧的值
          
          stack[num_of_stack-1]=0;
          if(val1==val2)
            temp=1;
          else temp=0;//比较结果
          tokens[tmp].type=NUMBER;
          sprintf(tokens[tmp].str,"%u",temp);
          count=tmp;
          num_of_stack--;
          break;//equal //注意，等号左侧的值要么包裹在一对括号中，要么就是一个数，都只占一个stack，neq和and都一样。
      case 248:
          tmp=0;
          for(i = count;i < q;i ++ )
          {
            if(tokens[i].type=='(')
            {
              tmp+=1;
            }
            else if(tokens[i].type==')')
            {
              tmp-=1;
            }
            if(tmp==0)
            {
              tmp=i;
              break;
            }
          }
          if(i == q)
              val2=calculate(count,q);
          else val2=calculate(count,tmp+1);//等号右侧的值
          
          val1=stack[num_of_stack-1];//计算左侧的值
          
          stack[num_of_stack-1]=0;
          if(val1==val2)
            temp=0;
          else temp=1;//比较结果
          tokens[tmp].type=NUMBER;
          sprintf(tokens[tmp].str,"%u",temp);
          count=tmp;
          num_of_stack--;
          break;//neq
      case 249:
          tmp=0;
          for(i = count;i < q;i ++ )
          {
            if(tokens[i].type=='(')
            {
              tmp+=1;
            }
            else if(tokens[i].type==')')
            {
              tmp-=1;
            }
            if(tmp==0)
            {
              tmp=i;
              break;
            }
          }
          if(i == q)
              val2=calculate(count,q);
          else val2=calculate(count,tmp+1);//等号右侧的值
          
          val1=stack[num_of_stack-1];//计算左侧的值

          stack[num_of_stack-1]=0;
          temp=val1&&val2; //结果
          tokens[tmp].type=NUMBER;
          sprintf(tokens[tmp].str,"%u",temp);
          count=tmp;
          num_of_stack--;
          break;//and
      case 251:
          flag=true;
          stack[num_of_stack++]=isa_reg_str2val(tokens[count-1].str,&flag);
          break;//寄存器的值
      case 250:
          tmp=0;
          for(i = count;i < q;i ++ )
          {
            if(tokens[i].type=='(')
            {
              tmp+=1;
            }
            else if(tokens[i].type==')')
            {
              tmp-=1;
            }
            if(tmp==0)
            {
              tmp=i;
              break;
            }
          }
          if(i == q) temp=calculate(count,q);
          else temp=calculate(count,tmp+1);
          tokens[tmp].type=NUMBER;
          sprintf(tokens[tmp].str,"%u",temp);
          count=tmp;
          break;//处理引用
      case 256://空格不需要操作
          break;
      case '+':
          stack[num_of_stack++]='+';
          sign[num_of_stack-1]='+';//加法压入栈
          break;
      case '-'://减法压入栈
          stack[num_of_stack++]='-';
          sign[num_of_stack-1]='-';
          break; 
      case '*'://乘法压入栈
          stack[num_of_stack++]='*';
          sign[num_of_stack-1]='*';
          break;
      case '/'://除法压入栈
          stack[num_of_stack++]='/';
          sign[num_of_stack-1]='/';
          break;
      case 252:
          if(num_of_stack == 0)  
            {
              sscanf(tokens[count-1].str,"%x", &temp);
              stack[num_of_stack++]=temp;
              break;
            }
            num_of_stack-=1;
          if(sign[num_of_stack]=='*'||sign[num_of_stack]=='/')
          {
            temp1=stack[num_of_stack-1];
            sscanf(tokens[count-1].str,"%x", &temp2);
            if(sign[num_of_stack]=='*')
            {
              temp1=temp1*temp2;
            }
            else{
              if(temp2==0)
              {
                printf("divided by 0\n");
                return 0;
              }
              temp1=temp1/temp2;
            }
            stack[num_of_stack-1]=temp1;
            sign[num_of_stack]=0;
            stack[num_of_stack]=0;
          }
          else{
            sscanf(tokens[count-1].str,"%u", &temp);
            stack[num_of_stack+1]=temp;
            num_of_stack+=2;
          }
          break;//遇到数字处理乘法和除法
      case 254:
          if(num_of_stack == 0)  
            {
              sscanf(tokens[count-1].str,"%u", &temp);
              stack[num_of_stack++]=temp;
              break;
            }
            num_of_stack-=1;
          if(sign[num_of_stack]=='*'||sign[num_of_stack]=='/')
          {
            temp1=stack[num_of_stack-1];
            sscanf(tokens[count-1].str,"%u", &temp2);
            if(sign[num_of_stack]=='*')
            {
              temp1=temp1*temp2;
            }
            else{
              if(temp2==0)
              {
                printf("divided by 0\n");
                return 0;
              }
              temp1=temp1/temp2;
            }
            stack[num_of_stack-1]=temp1;
            sign[num_of_stack]=0;
            stack[num_of_stack]=0;
          }
          else{
            sscanf(tokens[count-1].str,"%u", &temp);
            stack[num_of_stack+1]=temp;
            num_of_stack+=2;
          }
          break;//遇到数字处理乘法和除法
      case '(':
          stack[num_of_stack++]='(';
          sign[num_of_stack-1]='(';
          break;
      case ')':
          temp=0;
          num_of_stack-=1;
          while(sign[num_of_stack]!='(')
          {
            switch(sign[num_of_stack])
            {
              case '-':
                if(stack[num_of_stack-2]!='(' && sign[num_of_stack-2]==sign[num_of_stack])
              {
                temp=stack[num_of_stack-1]+temp;
                sign[num_of_stack]=0;
                stack[num_of_stack]=0;
                stack[num_of_stack-1]=temp;
              }     
                else if(stack[num_of_stack-2]!='('){
                temp=stack[num_of_stack-1]-temp;
                sign[num_of_stack]=0;
                stack[num_of_stack]=0;
                stack[num_of_stack-1]=temp;
              }
                else{
                temp=stack[num_of_stack-1]-temp;
                stack[num_of_stack-1]=temp;
              }
                sign[num_of_stack]=0;
                num_of_stack--;
                break;
              case '+':
                if(stack[num_of_stack-2]!='(' && sign[num_of_stack-2]==sign[num_of_stack])
                {
                  temp=stack[num_of_stack-1]+temp;
                  sign[num_of_stack]=0;
                  stack[num_of_stack]=0;
                  stack[num_of_stack-1]=temp;
                }
                else if(stack[num_of_stack-2]!='(')
                {
                  temp=stack[num_of_stack-1]-temp;
                  sign[num_of_stack]=0;
                  stack[num_of_stack]=0;
                  stack[num_of_stack-1]=temp;
                }
                else{
                  temp=stack[num_of_stack-1]+temp;
                  stack[num_of_stack-1]=temp;
                }
                sign[num_of_stack]=0;
                num_of_stack--;
                break;
              default:
                temp=stack[num_of_stack];
                stack[num_of_stack]=0;
                num_of_stack--;
            }
          }//括号里只有加减法，计算结果
          sign[num_of_stack]=0;
          count-=1;
          tokens[count].type=254;
          sprintf(tokens[count].str,"%u",temp); //把得到的值再次进行判断，处理a*(b+c)的情况
          break;   
      default:assert(0);
    }
  } //将所有的值的压入栈中。其中计算处理了括号中的数和乘除法
  temp=stack[--num_of_stack];
  while( num_of_stack > 0)
  {
    switch(sign[num_of_stack])
    {
      case '-':
        if(num_of_stack > 2 && sign[num_of_stack-2]==sign[num_of_stack])
        {
          temp=stack[num_of_stack-1]+temp;
          sign[num_of_stack]=0;
          stack[num_of_stack]=0;
          stack[num_of_stack-1]=temp;
        }
        else if(num_of_stack >2){
          temp=stack[num_of_stack-1]-temp;
          sign[num_of_stack]=0;
          stack[num_of_stack]=0;
          stack[num_of_stack-1]=temp;
        }
        else{
          temp=stack[num_of_stack-1]-temp;
          stack[num_of_stack-1]=temp;
        }
        num_of_stack--;
        break;
      case '+':
        if(num_of_stack > 2 && sign[num_of_stack-2]==sign[num_of_stack])
        {
          temp=stack[num_of_stack-1]+temp;
          sign[num_of_stack]=0;
          stack[num_of_stack]=0;
          stack[num_of_stack-1]=temp;
        }
        else if(num_of_stack >2)
        {
          temp=stack[num_of_stack-1]-temp;
          sign[num_of_stack]=0;
          stack[num_of_stack]=0;
          stack[num_of_stack-1]=temp;
        }
        else{
          temp=stack[num_of_stack-1]+temp;
          stack[num_of_stack-1]=temp;
        }
        num_of_stack--;
        break;
      default:
        temp=stack[num_of_stack];
        stack[num_of_stack]=0;
        num_of_stack--;
    }
  }//只有加减法的运算且无括号
  return temp;
}


/*
u_int32_t eval(int p,int q)
{
  u_int32_t temp=0,op,val1,val2;
  if(p > q)
  {
    printf("wrong!\n");
    return 0;
  }
  else if(p==q)
  {
    sscanf(tokens[p].str,"%u",&temp);
    return temp;
  }
  else if(check_parentheses(p,q)==true)
  {
    return eval(p+1,q-1);
  }
  else{

  }
}
*/