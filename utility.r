#zero one normalize
normalize.col.zerone <-function(mat){
  colmins <- apply(mat,MARGIN=2,FUN=min);
  colmaxs <- apply(mat,MARGIN=2,FUN=max);
  #print(colmins);
  #print(colmaxs);
  colnorm <- colmaxs - colmins;
  #print(colnorm)
  #print(1/colnorm)
  result <- apply(mat,1,'-',colmins);
  result <- t((1/colnorm)*(result));#apply(result,1,'/',colnorm);
  #print(result);
  return(result);
}

#for all the functions below;
#need stronger error tolerance
#empty matrix, missing value etc.
flipud<-function(m){
  x<-apply(m,2,rev);
  return(x)
}

#variance is always non-negative;
normalize.variance <-function(variance){
  if(!is.matrix(variance)&!is.data.frame(variance)&!is.vector(variance)){
    return(NULL);
  }
  len <- ncol(variance);
  g <- variance[1,len]; # model performance on the largest partition, starting offset 1, ending offset len;
  result <- ifelse(variance>g,(1-0.5*g/variance),(variance/g*0.5));
  return(result)
}

#bias can be positive or negative;
normalize.bias <- function(bias){
  #positive value under estimate
  #negative value over estimate
  # map the values to -max(abs(bias)) to max(abs(bias))
  if(!is.matrix(bias)&!is.data.frame(bias)&!is.vector(bias)){
    return(NULL);
  }
  len <- ncol(bias);
  
  valmax <- max(bias);
  
  valmin <- min(bias);
  
  gbias <- bias[1,len];
  
  bias.norm <- ifelse(bias>=gbias,(bias-gbias)/(valmax-gbias)*0.5+0.5,(bias-valmin)/(gbias-valmin)*0.5);
  #bias.norm <- (bias-valmin)/(valmax-valmin)*0.5+0.5; #bias>gbias
  #bias.norm <- (bias-valmin)/(valmax-valmin)*0.5; #bias < gbias;
  
  #gbias.norm <- bias.norm[1,len];
  
  return(bias.norm);
}

# model coverage 6 uses R squared as goodness measure;
# cut = 1, equal width
# cut = 2, equal frequency
# test variance;
model.coverage <- function(model, alldata, type=1, sampleGranu=15){

  temp.matrix <- as.matrix(alldata);
  
  coverage.list <- list();
  
  model.var.names <- colnames(model$model);
  
  y<-model.var.names[1];
  x<-model.var.names[c(2:(length(model.var.names)))];
  
  model.formula <- as.formula(paste(y, paste(x, collapse="+"), sep="~"));
  
  alldata.vars <- colnames(alldata);
  
  vars.ref.names <- alldata.vars;#[!alldata.vars%in%model.var.names];
  
  model.var.idx <- alldata.vars%in%model.var.names;
  #print(model.var.idx);
  
  model.var.matrix <- temp.matrix[,model.var.idx];
  #print(ncol(model.var.matrix))
  #print(nrow(model.var.matrix))
  #str(model.var.matrix);
  coef.colnum <- length(model.var.names);
  for(name in vars.ref.names){
    
    print(name);
    
    refvar <- as.vector(alldata[[name]]);
    
    if(type == 1){
      c <- cut(x=refvar,sampleGranu,include.lowest = TRUE);
    }
    
    if(type == 2){
      brks <- unique(quantile(refvar,probs=seq(0,1,length.out=sampleGranu)));
      c <- cut(x=refvar,breaks=brks,include.lowest = TRUE);
    }
    
    coef<-vector();

    uc <- levels(c);
    #print(uc);
    
    partition.idx <- matrix(data=NA, nrow = nrow(alldata), ncol = length(uc));
    
    for(k in 1:length(uc)){
      partition.idx[,k] <- c%in%uc[k];
    }
    if(sum(is.na(partition.idx))>0)
      print("error!");
    
    coverage <- matrix(data = 0, nrow = length(uc), ncol = length(uc));
    cell.range.lower <- matrix(data = 0, nrow = length(uc), ncol = length(uc));
    cell.range.upper <- matrix(data = 0, nrow = length(uc), ncol = length(uc));
    
    range <-sub(pattern="\\(",replacement="",uc);
    range <-sub(pattern="\\)",replacement="",range);    
    range <-sub(pattern="\\]",replacement="",range);
    range <-sub(pattern="\\[",replacement="",range);
    range <- strsplit(range,split=",");
    ranges <- do.call(rbind, range);
    mode(ranges) <- 'numeric';
    
    #print(partition.idx[,1]);
    for(i in 1:length(uc)){
      low <- ranges[i,1];
      #print(i);
      for(j in i:length(uc)){
        if(i==j){
          idx <- partition.idx[,i];
        }else{
          idx <- as.logical(rowSums(partition.idx[,c(i:j)]));
        }
        high <- ranges[j,2];
        if(sum(idx)>2){
          model.var.mtx.subset <- model.var.matrix[idx,];
          model.var.df.subset <- as.data.frame(model.var.mtx.subset);
          trained<-lm(model.formula,data=model.var.df.subset);
          trained.summary <- summary(trained);
          rsquare <- trained.summary$r.squared;
          #print(rsquare);
          coverage[i,j] <- rsquare;
          coverage[j,i] <- rsquare;

          coef <-rbind(coef,trained$coefficients); 
          #print(trained$coefficients);
        }else{
          coverage[i,j] <- 0;
          coverage[j,i] <- 0;
          
          #coef <-rbind(coef,coeff.na);
          
          coeff.na<-as.vector(matrix(data=0,nrow=1,ncol=coef.colnum));
          coef <-rbind(coef, coeff.na);
          #print(coeff.na);
        }
        cell.range.lower[i,j] <- low
        cell.range.lower[j,i] <- low
        
        cell.range.upper[i,j] <- high
        cell.range.upper[j,i] <- high
      }
    }
    
    var.coverage <- list("coverage"=coverage,"name"= name,"coeff"=coef, "cut.low"=cell.range.lower,"cut.high"=cell.range.upper);
    
    coverage.list <- c(coverage.list,list(var.coverage));
  }
  names(coverage.list) <- vars.ref.names;
  return(coverage.list)
}

# model.coverage <- function(model, alldata, cut="2", sampleGranu=10){
#   
#   temp.matrix <- as.matrix(alldata);
#   
#   coverage.list <- list();
#   
#   model.var.names <- colnames(model$model);
#   
#   y<-model.var.names[1];
#   x<-model.var.names[c(2:(length(model.var.names)))];
#   
#   model.formula <- as.formula(paste(y, paste(x, collapse="+"), sep="~"));
#   
#   alldata.vars <- colnames(alldata);
#   
#   vars.ref.names <- alldata.vars[!alldata.vars%in%model.var.names];
#   
#   model.var.idx <- alldata.vars%in%model.var.names;
#   #print(model.var.idx);
#   
#   model.var.matrix <- temp.matrix[,model.var.idx];
#   #print(ncol(model.var.matrix))
#   #print(nrow(model.var.matrix))
#   #str(model.var.matrix);
#   
#   for(name in vars.ref.names){
#     
#     print(name);
#     
#     refvar <- as.vector(alldata[[name]]);
#     
#     if(cut == "1"){
#       c <- cut(refvar,sampleGranu,include.lowest = TRUE);
#     }
#     
#     if(cut == "2"){
#       brks <- unique(quantile(refvar,probs=seq(0,1,length.out=sampleGranu)));
#       c <- cut(x=refvar,breaks=brks,include.lowest = TRUE);
#     }
#     
#     coef<-vector();
#     
#     uc <- levels(c);
#     
#     partition.idx <- matrix(data=NA, nrow = nrow(alldata), ncol = length(uc));
#     
#     for(k in 1:length(uc)){
#       partition.idx[,k] <- c%in%uc[k];
#     }
#     if(sum(is.na(partition.idx))>0)
#       print("error!");
#     
#     coverage <- matrix(data = 0, nrow = length(uc), ncol = length(uc));
#     cell.range.lower <- matrix(data = 0, nrow = length(uc), ncol = length(uc));
#     cell.range.upper <- matrix(data = 0, nrow = length(uc), ncol = length(uc));
#     
#     range <-sub(pattern="\\(",replacement="",uc);
#     range <-sub(pattern="\\)",replacement="",range);    
#     range <-sub(pattern="\\]",replacement="",range);
#     range <-sub(pattern="\\[",replacement="",range);
#     range <- strsplit(range,split=",");
#     ranges <- do.call(rbind, range);
#     mode(ranges) <- 'numeric';
#     
#     #print(partition.idx[,1]);
#     trained<-lm(model.formula,data=alldata);
#     #trained.summary <- summary(trained);
#     #rsquare <- trained.summary$r.squared;
#     
#     for(i in 1:length(uc)){
#       low <- ranges[i,1];
#       for(j in i:length(uc)){
#         if(i==j){
#           idx <- partition.idx[,i];
#         }else{
#           idx <- as.logical(rowSums(partition.idx[,c(i:j)]));
#           #str(idx);
#         }
#         high <- ranges[j,2];
#         res <- trained$residuals[idx];
#         pred <-trained$fitted.values[idx];
#         y <- res+pred;
#         total <- sum((y-mean(y))^2);
#         ssres <- sum(res^2);
#         rsquare <- 1-ssres/total;
#         if(rsquare<0)rsquare=0;
#         #model.var.mtx.subset <- model.var.matrix[idx,];
#         #model.var.df.subset <- as.data.frame(model.var.mtx.subset);
#         
#         #trained<-lm(model.formula,data=model.var.df.subset);
#         #trained.summary <- summary(trained);
#         #rsquare <- trained.summary$r.squared;
#         
#         coverage[i,j] <- rsquare;
#         coverage[j,i] <- rsquare;
#         
#         #cell.range.lower[i,j] <- low
#         #cell.range.lower[j,i] <- low
#         
#         #cell.range.upper[i,j] <- high
#         #cell.range.upper[j,i] <- high
#         
#         #coef <-rbind(coef,trained$coefficients);
#       }
#     }
#     
#     var.coverage <- list("coverage"=coverage,"name"= name,"coeff"=coef, "cut.low"=cell.range.lower,"cut.high"=cell.range.upper);
#     
#     coverage.list <- c(coverage.list,list(var.coverage));
#   }
#   names(coverage.list) <- vars.ref.names;
#   return(coverage.list)
# }


model.diversity <- function(model.coverage.status){
  diversity.vec <- list();
  sd.vec <- vector();
  for(coverage in model.coverage.status){
    sd.vec <- c(sd.vec,max(as.vector(coverage$coverage)));
    reminder <- coverage$coverage;
    diag.vec <- vector();
    while(length(reminder)>0){
      if(length(reminder)==1){
        diag.vec<-c(diag.vec,reminder);
        break;
      }else{
        diag.vec <- c(diag.vec,diag(reminder));
        temp <- matrix(NA,ncol(reminder)-1,ncol(reminder)-1);
        temp[upper.tri(temp,TRUE)] <- reminder[upper.tri(reminder, FALSE)];
        reminder <- temp; #reduce rank by 1;
        #diag.vec <- c(diag.vec,diag(utri));
        #print(ncol(reminder));
      }
    }
    diversity.vec <- c(diversity.vec,list(diag.vec));
  }
  names(diversity.vec)<- names(model.coverage.status);
  
  diversity.names <- names(diversity.vec);
  
  diversity.rank <- order(-sd.vec);
  
  result <- list("diversity.vec"=diversity.vec,"diversity.rank"=diversity.rank,"diversity.names"=diversity.names);
  
  return(result);
}





#compute robust correlation
# x is the independent variable
# y is the target variable

# cut = 1: by value;
# cut = 2: by frequency
# return the correlation score of 
# the most correlated partition;
localfit.1d<-function(x,y,cut="2",b = 7){
  
  result <- vector();
  
  if(b>1){
    if(cut == "1"){
      c <- cut(x,b,include.lowest = TRUE);
    }
    if(cut == "2"){
      brks <- unique(quantile(x,probs=seq(0,1,length.out=b)));
      c <- cut(x=x,breaks=brks,include.lowest = TRUE);
    }
    
    for(i in levels(c)){
      idx <- c%in%i;
      if(sum(idx)<1){
        result <- cbind(result,0);
      }else{
        locx <- x[idx];
        locy <- y[idx];
        fit <- lm(locy~locx);
        ssres <- sum(fit$residuals^2);
        sstotal<- sum((locy-mean(locy))^2);
        rsquare <- 1-ssres/sstotal;
        result <- cbind(result,rsquare); 
      }
    }
    t <- max(result,na.rm=TRUE);
    #w <- which.max(x=result);
    ranges <- levels(c);
    
    r<-ranges;
    r <-sub(pattern="\\(",replacement="",r);
    r <-sub(pattern="\\)",replacement="",r);
    
    r <-sub(pattern="\\]",replacement="",r);
    r <-sub(pattern="\\[",replacement="",r);
    
    r <- strsplit(r,split=",");
    r <- do.call(rbind, r);
    mode(r) <- 'numeric';
    ranges.mid <- rowMeans(r);
  }else{
    ranges <- "";
    ranges.mid <- 0;
    fit <- lm(y~x);
    ssres <- sum(fit$residuals^2);
    sstotal<- sum((y-mean(y))^2);
    t <- 1-ssres/sstotal;
    #w<-1;
  }
  l<-list("max"=t,"ranges"=ranges,"ranges.mid"=ranges.mid,"rsqr.list"=result);
  return(l)
  #print(result);cor()x,y
  #print(m);
  #result <- cbind(result,m);
  #result <- cbind(result,abs(cor(x,y)));
  
  #if(m>t){
  # print(m);
  # print(t);
  #}
  # print(result);
  #return(result)
}


localfit.rank <- function(df){
  len <- ncol(df);
  target.name <- colnames(df)[len];
  ind.names <- colnames(df)[1:(len-1)];
  
  r <- NULL;
  
  for(name in ind.names){
    x <- df[[name]];
    y <- df[[target.name]];
    fit <- lm(y~x);
    r <- c(r,summary(fit)$r.square);
  }
  o <- order(-r);
  result <- list("order"=o,"rsquare"=r,"names"=ind.names);
}

#relavance Ranking 
# cut = 1: by value;
# cut = 2: by frequency
# max number of bins
# localfit.rank <- function(df,ctype="2",maxbin=7,filter=0.0){
#   
#   len <- ncol(df);
#   
#   target <- df[,len];
#   
#   #locmax <- NULL;
#   
#   scores <- matrix(data=NA,nrow=len-1,ncol=maxbin);
#   
#   #r <- matrix(data=NA,nrow=len-1,ncol=maxbin);
#   
#   #ridx <- matrix(data=NA,nrow=len-1,ncol=maxbin);
#   
#   var.names <- colnames(df);
#   
#   lfit.list <- list();
#   lrange.list <- list();
#   lmid.list <- list();
#   
#   for(i in 1:(len-1)){
#     # the i th variable
#     lfit.var.list <-list();
#     lrange.var.list <- list();
#     lmid.var.list <- list();
#     
#     for(j in 1:maxbin){
#       
#       s <- localfit.1d(df[,i],target,cut=ctype,b=(j+1));
#       
#       scores[i,j] <- s$max;
#       
#       lfit.var.list[[j]] <- as.vector(s$rsqr.list);
#       lrange.var.list[[j]] <- s$ranges;
#       lmid.var.list[[j]] <- s$ranges.mid;
#     }
#     #names(var.list) <- c(1:maxbin);
#     lfit.list[[i]] <- lfit.var.list;
#     lrange.list[[i]] <- lrange.var.list;
#     lmid.list[[i]]<- lmid.var.list;
#   }
#   names(lfit.list) <- var.names[1:(len-1)];
#   names(lrange.list) <- var.names[1:(len-1)];
#   names(lmid.list) <- var.names[1:(len-1)];
#   #idx <-rowSums(scores>filter)>0;
#   
#   #filtered.score<-scores[idx,];
#   
#   #filtered.range<-r[idx,];
#   
#   #filtered.range.idx <- ridx[idx,];
#   rowmax<-apply(scores,1,max);
#   
#   #rowmax <- max(filtered.score);
#   
#   o <- order(-rowmax);
#   
#   #names <-colnames(df)[idx];
#   
#   l <- list("scores"= scores,"ranges"=lrange.list,"ranges.mid"=lmid.list,"lfit.list"=lfit.list,"order"=o,"names"=var.names);
#   
#   return(l);
#   #o <- order(-locmax);
#   #result <- list("order"=o,"locCor"=scores);
# }


#train a global model;
model.train <- function(var.vector,df){
  sname <- colnames(df);
  len <- length(sname);
  
  y<-sname[length(sname)];
  x<-sname[var.vector];
  
  mymodel <- as.formula(paste(y, paste(x, collapse="+"), sep="~"));
  trained.model<-lm(mymodel,data=df);
  return(trained.model);
}

# process the list of models, and return vector of coeffs
# RMSE, RSquare for each model;
model.status <- function(model.list,varRank){
  
  var.names <- varRank$names[varRank$order];
  
  status.list <- list();
  
  for(m in model.list){
    
    status <- list();
    
    status <-c(status, list(m$coefficients));
    
    status <-c(status, list(names(m$coefficients)));
    
    coeff.idx <- which(var.names%in%names(m$coefficients));
    
    status <- c(status,list(coeff.idx));
    
    r <- m$residuals;
    rmse <- sqrt(sum(r^2)/length(r));
    status <-c(status, rmse);
    
    y <- r+m$fitted.values;
    sstotal<- sum((y-mean(y))^2);
    ssres <- sum(r^2);
    rsquare <- 1-ssres/sstotal;
    status <-c(status, rsquare);
    
    names(status)<-c("coefficients","coefficients.names","coefficients.idx","RMSE","RSQ");
    
    status.list <-c(status.list,list(status));
  }
  
  return(status.list);
}

coeff.clustering <- function(model.coverage, var.list, meth="ward"){
  model.coverage.selected <- model.coverage[var.list];
  coeff <- NULL;
  coeff.size <- NULL;
  vec.list <- NULL;
  
  i.int <- 1; #coeff index;
  for(i in model.coverage.selected){
    coeff <- rbind(coeff,i$coeff)
    coeff.size <- c(coeff.size,nrow(i$coeff));
    #coeff row indices;
    i.coeff.rows <- c(1:nrow(i$coeff));
    for(j in (i.coeff.rows)){
      vec <- c(i.int-1,j-1); # which grid, which cell;
      vec.list <- c(vec.list,vec);
    }
    i.int<-i.int+1;
  }
  coeff.norm <- normalize.col.zerone(coeff);
  clust <- hclust(dist(coeff.norm),method=meth);
  #clust$merge
  l<-list("coeff"=coeff,"coeff.norm"=coeff.norm,"coeff.name"=colnames(coeff),"clust.tree"=as.dendrogram(clust),"index"=vec.list);
  return(l);
  #return a list of vectors, each vector has two elements; 
  #first element x indicates which variable the coverage is based on;
  #second element y indicates which partition the model is built regarding the x;
}

coeff.fivenum<-function(coeff.cluster,member.idx){
  temp1 <- coeff.cluster$coeff.norm[member.idx,];
  #print(temp1);
  
  coeff.fivenum.norm <- apply(temp1,2,fivenum);
  #print(coeff.fivenum.norm);
  
  temp2 <- coeff.cluster$coeff[member.idx,];
  coeff.fivenum <- apply(temp2,2,fivenum);
  
  coeff.name <- coeff.cluster$coeff.name;
  l <- list("coeff"=coeff.fivenum,"coeff.norm" = coeff.fivenum.norm,"coeff.name"=coeff.name);
  #return centroid coefficient, and centroid coefficient names;
}

locfit.boxplot <- function(df,model.list,model.idx=1,v2,cut=10,type=2){
  #df, data frame
  #v1, name variable 1;
  #v2, name variable 2;
  len <- ncol(df);
  target <- df[[len]];
  if(is.null(model.list))return(NULL);
  if(length(model.list)<model.idx)return(NULL);
  model1 <- model.list[[model.idx]];
  e1 <- model1$residuals;  
  x2 <- df[[v2]];
  #model1.formula <- formula(terms(model.list[[1]]));
  #model1.
  
  if(type==1){
    par <- cut(x2,breaks=cut,include.lowest=TRUE);
  }
  if(type==2){
    brks <- unique(quantile(x2,probs=seq(0,1,length.out=(cut+1))));
    par <- cut(x=x2,breaks=brks,include.lowest = TRUE);
  }
  #model difference;
      
  #x2 <- df[[v2]];
  fit2 <- lm(target~x2);
  e2 <- fit2$residuals;
  #diff.base <- abs(e1)+abs(e2);
  #diff.base[diff.base<0.001] = e2[diff.base<0.001];
  #diff <- e2/diff.base;
  
  b1<-boxplot(e1~par,outline=FALSE,plot=FALSE);
  b2<-boxplot((e2)~par,outline=FALSE,plot=FALSE);
  
  
  ranges <- levels(par);
  
  r<-ranges;
  r <-sub(pattern="\\(",replacement="",r);
  r <-sub(pattern="\\)",replacement="",r);
  
  r <-sub(pattern="\\]",replacement="",r);
  r <-sub(pattern="\\[",replacement="",r);
  
  r <- strsplit(r,split=",");
  r <- do.call(rbind, r);
  mode(r) <- 'numeric';
  ranges.mid <- rowMeans(r);
  
  s1 <- b1$stats[,as.logical(colSums(!is.na(b1$stats)))];
  s2 <- b2$stats[,as.logical(colSums(!is.na(b2$stats)))];
  r <- ranges.mid[as.logical(colSums(!is.na(b1$stats)))];
  #"ranges"= levels(par);
  xmin <- min(ranges.mid,na.rm=TRUE)-0.05*(max(ranges.mid,na.rm=TRUE)-min(ranges.mid,na.rm=TRUE));
  xmax <- max(ranges.mid,na.rm=TRUE)+0.05*(max(ranges.mid,na.rm=TRUE)-min(ranges.mid,na.rm=TRUE));
  statsmin <- min(b1$stats,b2$stats,na.rm=TRUE);
  statsmax <- max(b1$stats,b2$stats,na.rm=TRUE);
  ymin <- statsmin-0.05*(statsmax-statsmin);
  ymax <- statsmax+0.05*(statsmax-statsmin);
  result <- list("stats.current"=s1,"stats"=s2,"ranges"= ranges.mid,"xmax"=xmax,"xmin"=xmin,"ymax"=ymax,"ymin"=ymin);
  #result <- list("bar1.top"=b$stats[5,],"bar1.bottom"=b$stats[1,],"bar2.top"=b$stats[4,],"bar2.bottom"=b$stats[2,],"mid"=b$stats[3,]);
  return(result);
}
