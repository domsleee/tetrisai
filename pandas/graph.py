import pandas as pd
import plotly.express as px
import plotly.graph_objects as go

df = pd.read_csv('../cpp/results/bfs/19_Bfs.res')
df2 = pd.DataFrame(dict(probability=[1.0-(i/len(df)) for i in range(len(df))]))

df_branch = pd.read_csv('../cpp/results/bfs/19_BfsBranch.res')
df2_branch = pd.DataFrame(dict(probability=[1.0-(i/len(df)) for i in range(len(df_branch))]))

MAXOUT_SCORE = 999999
MAX_SCORE = 1542000


fig = go.Figure()
fig.add_trace(go.Scatter(x=df.score, y=df2.probability, mode='lines+markers', name='Bfs'))
fig.add_trace(go.Scatter(x=df_branch.score, y=df2_branch.probability, mode='lines+markers', name='Bfs+branching'))


fig.add_trace(go.Scatter(
  x=[MAXOUT_SCORE, MAXOUT_SCORE],
  y=[0, 1],
  name=f"Maxout score ({MAXOUT_SCORE})",
  mode="lines+text",
  text=["Maxout score"],
  textposition="bottom center",
  line=dict(
    color="grey",
    width=3,
    dash="dash"
  ),
  hoverinfo='skip'
))

fig.add_trace(go.Scatter(
  x=[MAX_SCORE, MAX_SCORE],
  y=[0, 1],
  name=f"Max score ({MAX_SCORE})",
  line=dict(
    color="grey",
    width=3,
    dash="dash"
  ),
  mode="lines+text",
  text=["Max score"],
  textposition="bottom center",
  hoverinfo='skip'
))

fig.update_layout(
  title='Score vs probability',
  yaxis_title='probability',
  xaxis_title='score',
  legend=dict(
    x=0.8,
    y=0.9,
    traceorder="normal",
    font=dict(
        family="sans-serif",
        size=12,
        color="black"
    ),
    bgcolor="LightSteelBlue",
    bordercolor="Black",
    borderwidth=2
  )
)

#fig = px.scatter(df,  mode='lines+trace', x='score', y=df2.probability, title='Score vs probability', labels={'y': 'probability'})
fig.show()
fig.write_html("out.html")
