using AutoMapper;
using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Data.Entity.Infrastructure;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Web.Http;
using System.Web.Http.Description;
using Trivio.Models;
using Trivio.Models.DTOs;

namespace Trivio.Controllers
{
	[RoutePrefix("Trivia")]
    public class TriviaController : ApiController
    {
        private TrivioContext db = new TrivioContext();

		//GET: Trivia/{UserId}
		[HttpGet,Route("{userId}")]
		[ResponseType(typeof(string))]
		public async Task <IHttpActionResult> GetTrivia(int userId)
		{
			User user = await db.Users.FindAsync(userId);
			if (user == null)
			{
				return NotFound();
			}

			//Increment trivia count and return trivia at new row
			try {
				string trivia = (await db.Trivias.FindAsync(++user.TriviaCount)).Text;
				await db.SaveChangesAsync();
				return Ok(trivia);
			}
			catch
			{
				return NotFound();
			}
			
		}

		//PUT: Trivia/Vote
		[HttpPut,Route("Vote/{id}")]
		[ResponseType(typeof(UserTriviaHistoryDTO))]
		public async Task<IHttpActionResult> VoteOnTrivia(int id, VoteDTO vote)
		{
			if (!ModelState.IsValid)
			{
				return BadRequest(ModelState);
			}

			if (id != vote.TriviaId)
			{
				return BadRequest();
			}


			//Download the correct User Trivia history
			var userTriviaHistory = await db.UserTriviaHistories
										.Include(p=>p.User)
										.Include(p=>p.Trivia)
										.FirstOrDefaultAsync(x => x.TriviaId == vote.TriviaId && x.UserId == vote.UserId);

													
			//User has not heard the trivia yet or invlaid user id/triviaid
			if(userTriviaHistory ==null)
			{
				return BadRequest("User has not heard trivia yet or invalid user id/trivia id ");
			}

			Trivia trivia = userTriviaHistory.Trivia;

			//If new vote == old vote
			int currentVote = userTriviaHistory.UserVote;
			if(vote.UserVote == currentVote) {
				return StatusCode(HttpStatusCode.NoContent);
			}

			//If vote is different than current vote, update both Upvotes and Downvotes
			if(vote.UserVote==-1) 
			{
				trivia.Downvotes++;
				if (currentVote == 1) trivia.Upvotes--;
			}
			else if(vote.UserVote==1)
			{
				trivia.Upvotes++;
				if (currentVote == -1) trivia.Downvotes--;
			}
			else if (vote.UserVote == 0)
			{
				trivia.Upvotes -= (currentVote==1) ? 1 : 0;
				trivia.Downvotes -= (currentVote == -1)? 1 : 0;
			}
			userTriviaHistory.UserVote = vote.UserVote;

			//Update database
			db.Entry(trivia).State = EntityState.Modified;
			db.Entry(userTriviaHistory).State = EntityState.Modified;

			try
			{
				await db.SaveChangesAsync();
			}
			catch (DbUpdateConcurrencyException)
			{
				if (!TriviaExists(id))
				{
					return NotFound();
				}
				else
				{
					throw;
				}
			}

			//Return updated value
			return Ok(Mapper.Map<UserTriviaHistoryDTO>(userTriviaHistory));
		}


		//---------------------------------------------------------------------------------------
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }

        private bool TriviaExists(int id)
        {
            return db.Trivias.Count(e => e.Id == id) > 0;
        }
    }
}